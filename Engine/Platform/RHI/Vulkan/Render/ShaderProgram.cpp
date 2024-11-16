/**
 * @file ShaderProgram.cpp
 * @author Echo 
 * @Date 24-4-29
 * @brief 
 */

#include "ShaderProgram.h"

#include "Core/CoreEvents.h"
#include "Core/CoreGlobal.h"
#include "Platform/FileSystem/File.h"
#include "Core/Misc/Vertex.h"
#include "Platform/PlatformLogcat.h"

#include <ranges>

#include "Platform/RHI/Vulkan/Resource/Buffer.h"
using namespace platform;

namespace rhi::vulkan
{
class ShaderProgramManager
{
    struct ShaderPath
    {
        platform::File vert;
        platform::File frag;
    };

    struct ShaderPathHash
    {
        size_t operator()(const ShaderPath& p) const
        {
            return std::hash<core::String>()(p.vert.GetAbsolutePath()) ^ std::hash<core::String>()(p.frag.GetAbsolutePath());
        }
    };

    struct ShaderPathEqual
    {
        bool operator()(const ShaderPath& p1, const ShaderPath& p2) const { return p1.vert == p2.vert && p1.frag == p2.frag; }
    };

public:
    static ShaderProgram* Request(const File& v, const File& f)
    {
        ShaderPath p;
        p.vert = v;
        p.frag = f;
        if (shader_programs_.contains(p))
        {
            return shader_programs_[p];
        }
        return nullptr;
    }

    static void Register(ShaderProgram* prog)
    {
        if (shader_programs_.empty())
        {
            VulkanContext::Get()->OnPreVulkanDeviceDestroyed.AddBind(&ShaderProgramManager::DestroyAll);
        }
        ShaderPath p;
        p.vert              = prog->GetVertShader()->GetShaderPath();
        p.frag              = prog->GetFragShader()->GetShaderPath();
        shader_programs_[p] = prog;
    }

    static void DestroyAll()
    {
        for (const auto& v: shader_programs_ | std::views::values)
        {
            Delete(v);
        }
        shader_programs_.clear();
    }

protected:
    inline static core::HashMap<ShaderPath, ShaderProgram*, ShaderPathHash, ShaderPathEqual> shader_programs_;
};

ShaderProgram::ShaderProgram(
    LogicalDevice* device, Shader* vert, Shader* frag, const EShaderDestroyTime destroy_time, const core::String& debug_name
) : vert_shader_(vert), frag_shader_(frag), destroy_time_(destroy_time), device_(device)
{
    vertex_input_attributes_ = vert->GetInAttributes();
    // 校验VertexShader和FragmentShader的uniform变量
    if (!CheckAndUpdateUniforms(vert))
    {
        return;
    }
    if (!CheckAndUpdateUniforms(frag))
    {
        return;
    }

    name_ = debug_name;
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();
}

ShaderProgram::~ShaderProgram()
{
    DestroyDescriptorSets();
    DestroyDescriptorSetLayout();
    DestroyDescriptorPool();
    DestroyUniformBuffers();
}

ShaderProgram* ShaderProgram::Create(Shader* vert, Shader* frag, const EShaderDestroyTime destroy_time, const core::String& debug_name)
{
    auto& device = VulkanContext::Get()->GetLogicalDevice();
    auto* prog   = ShaderProgramManager::Request(vert->GetShaderPath(), frag->GetShaderPath());
    if (prog != nullptr)
    {
        return prog;
    }
    prog = New<ShaderProgram>(device.Get(), vert, frag, destroy_time, debug_name);
    ShaderProgramManager::Register(prog);
    return prog;
}

bool ShaderProgram::CheckAndUpdateUniforms(const Shader* shader)
{
    for (const auto& uniform: shader->GetUniformObjects())
    {
        if (uniforms_.contains(uniform.name))
        {
            if (uniform.binding != uniforms_[uniform.name].binding)
            {
                LOGGER.Error(logcat::Platform_RHI_Vulkan, "Different uniform binding: Name: {}", uniform.name);
                return false;
            }
        }
        else
        {
            uniforms_[uniform.name] = uniform;
        }
    }
    return true;
}

core::Array<vk::VertexInputAttributeDescription> ShaderProgram::GetVertexInputAttributeDescriptions() const
{
    core::Array<vk::VertexInputAttributeDescription> AttributeDesc;
    for (const auto& Attribute: vertex_input_attributes_)
    {
        vk::VertexInputAttributeDescription Desc;
        // clang-format off
        Desc
            .setBinding(0)
            .setLocation(Attribute.location)
            .setOffset(Attribute.offset);
        if (Attribute.size == 4) {
            Desc.setFormat(vk::Format::eR32Sfloat);
        } else if (Attribute.size == 8) {
            Desc.setFormat(vk::Format::eR32G32Sfloat);
        } else if (Attribute.size == 12) {
            Desc.setFormat(vk::Format::eR32G32B32Sfloat);
        } else if (Attribute.size == 16) {
            Desc.setFormat(vk::Format::eR32G32B32A32Sfloat);
        }
        // clang-format on
        AttributeDesc.push_back(Desc);
    }
    return AttributeDesc;
}

core::Array<vk::VertexInputBindingDescription> ShaderProgram::GetVertexInputBindingDescription() const
{
    core::Array<vk::VertexInputBindingDescription> binding_descriptions;
    vk::VertexInputBindingDescription              desc{};
    // clang-format off
    desc
        .setBinding(0)
        .setStride(GetStride())
        .setInputRate(vk::VertexInputRate::eVertex);
    // clang-format on
    binding_descriptions.push_back(desc);
    return binding_descriptions;
}

uint32_t ShaderProgram::GetStride() const
{
    // Mesh里Vertex的大小
    return sizeof(Vertex);
}

bool ShaderProgram::SetTexture(const core::String& name, const ImageView& view, const Sampler& sampler)
{
    if (!uniforms_.contains(name))
    {
        return false;
    }
    if (!view.IsValid())
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "ShaderProgram::SetTexture: ImageView {} is invalid in ShaderProgram {}", name, name_);
        return false;
    }
    if (!sampler.IsValid())
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "ShaderProgram::SetTexture: Sampler {} is invalid in ShaderProgram {}", name, name_);
        return false;
    }
    if (uniform_texture_storage_.contains(name))
    {
        if (const auto& storage = uniform_texture_storage_[name]; storage.view == &view && storage.sampler == &sampler)
        {
            return true;
        }
    }
    vk::WriteDescriptorSet descriptor_write;
    const auto&            tex_uniform = uniforms_[name];
    descriptor_write.dstBinding        = tex_uniform.binding;
    descriptor_write.dstArrayElement   = 0;
    descriptor_write.descriptorType    = vk::DescriptorType::eCombinedImageSampler;
    descriptor_write.descriptorCount   = 1;
    vk::DescriptorImageInfo image_info;
    image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    image_info.imageView   = view.GetHandle();
    image_info.sampler     = sampler.GetHandle();
    for (const auto descriptor_set: descriptor_sets_)
    {
        descriptor_write.dstSet     = descriptor_set;
        descriptor_write.pImageInfo = &image_info;
        VulkanContext::Get()->GetLogicalDevice()->UpdateDescriptorSets(descriptor_write);
    }
    TextureStorage storage(&view, &sampler);
    uniform_texture_storage_.insert({name, storage});
    return true;
}

void ShaderProgram::SetUniformBuffer(const core::String& name, const void* data, size_t size)
{
    if (!uniform_buffers_.contains(name))
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "ShaderProgram::SetUniformBuffer: UniformBuffer {} not found", name);
        return;
    }
    auto& buffers = uniform_buffers_[name];
    for (size_t i = 0; i < g_engine_statistics.graphics.parallel_render_frame_count; i++)
    {
        auto& buffer = buffers[i];
        buffer->MapMemory();
        buffer->GetMappedCpuMemory();
        buffer->Memcpy(data, size);
        // buffer->FlushMemory();
    }
}

bool ShaderProgram::HasShaderUniform(const core::String& name) const
{
    return uniforms_.contains(name);
}

const core::Array<PushConstantDescriptor>& ShaderProgram::GetVertexPushConstants() const
{
    return vert_shader_->GetPushConstantDescriptors();
}

const core::Array<PushConstantDescriptor>& ShaderProgram::GetFragmentPushConstants() const
{
    return frag_shader_->GetPushConstantDescriptors();
}

bool ShaderProgram::SetCubeTexture(const core::String& name, const ImageView& image_view, const Sampler& sampler)
{
    if (!uniforms_.contains(name))
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "Not find cubemap sampler {} in ShaderProgram {}", name, name_);
        return false;
    }
    if (!sampler.IsValid())
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "Sampler {} is invalid when update cubemap sampler in ShaderProgram {}", name, name_);
        return false;
    }
    if (!image_view.IsValid())
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "ImageView {} is invalid when update cubemap sampler in ShaderProgram {}", name, name_);
        return false;
    }
    if (uniform_texture_storage_.contains(name))
    {
        auto& storage = uniform_texture_storage_[name];
        if (storage.view == &image_view && storage.sampler == &sampler) return true;
    }
    vk::DescriptorImageInfo image_info;
    image_info.sampler     = sampler.GetHandle();
    image_info.imageView   = image_view.GetHandle();
    image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    core::Array<vk::WriteDescriptorSet> write_descriptor_sets;
    for (const auto& descriptor_set: descriptor_sets_)
    {
        vk::WriteDescriptorSet write_descriptor_set;
        write_descriptor_set.dstSet          = descriptor_set;
        write_descriptor_set.dstBinding      = uniforms_[name].binding;
        write_descriptor_set.dstArrayElement = 0;
        write_descriptor_set.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
        write_descriptor_set.descriptorCount = 1;
        write_descriptor_set.pImageInfo      = &image_info;
        write_descriptor_sets.push_back(write_descriptor_set);
    }
    VulkanContext::Get()->GetLogicalDevice()->UpdateDescriptorSets(write_descriptor_sets);
    TextureStorage storage(&image_view, &sampler);
    uniform_texture_storage_.insert({name, storage});
    return true;
}

bool ShaderProgram::IsValid() const
{
    return descriptor_set_layout_ != nullptr && !descriptor_sets_.empty() && descriptor_pool_ != nullptr;
}

void ShaderProgram::CreateUniformBuffers()
{
    // 静态共享的VP矩阵的UniformBuffer
    for (const auto& value: GetUniforms() | std::views::values)
    {
        if (value.type == EUniformDescriptorType::UniformBuffer || value.type == EUniformDescriptorType::DynamicUniformBuffer)
        {
            core::Array<Buffer*> new_buffer(g_engine_statistics.graphics.parallel_render_frame_count);
            for (size_t i = 0; i < g_engine_statistics.graphics.parallel_render_frame_count; i++)
            {
                new_buffer[i] = New<Buffer>(
                    value.size,
                    vk::BufferUsageFlagBits::eUniformBuffer,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                    name_ + "_" + value.name
                );
            }
            uniform_buffers_[value.name] = new_buffer;
        }
    }
}

void ShaderProgram::DestroyUniformBuffers()
{
    for (auto& value: uniform_buffers_ | std::views::values)
    {
        for (auto& buffer: value)
        {
            Delete(buffer);
        }
    }
    uniform_buffers_.clear();
}

void ShaderProgram::CreateDescriptorSets()
{
    if (descriptor_set_layout_ == nullptr)
    {
        CreateDescriptorSetLayout();
    }
    VulkanContext&                context = *VulkanContext::Get();
    core::Array                   layouts(g_engine_statistics.graphics.parallel_render_frame_count, descriptor_set_layout_);
    vk::DescriptorSetAllocateInfo alloc_info = {};
    alloc_info.setDescriptorPool(descriptor_pool_).setSetLayouts(layouts);
    // 描述符池对象销毁时会自动清除描述符集
    descriptor_sets_ = context.GetLogicalDevice()->AllocateDescriptorSets(alloc_info);

    if (!name_.IsEmpty())
    {
        for (size_t i = 0; i < descriptor_sets_.size(); i++)
        {
            descriptor_set_names_.emplace_back(name_ + "_DescriptorSet_" + std::to_string(i));
            context.GetLogicalDevice()->SetDescriptorSetDebugName(descriptor_sets_[i], descriptor_set_names_.back().Data());
        }
    }

    // 创建材质时首先使用默认丢失的贴图，之后需要调用更新贴图的方法
    const auto& default_lack_texture_view = Texture::GetDefaultLackTextureView();
    const auto& sampler                   = Sampler::GetDefaultSampler();
    for (size_t i = 0; i < descriptor_sets_.size(); i++)
    {
        core::Array<vk::WriteDescriptorSet>   descriptor_writes = {};
        core::Array<vk::DescriptorBufferInfo> buffer_infos;
        core::Array<vk::DescriptorImageInfo>  image_infos;

        buffer_infos.reserve(uniforms_.size());
        image_infos.reserve(uniforms_.size());

        for (auto& uniform: uniforms_ | std::views::values)
        {
            if (uniform.update_manually)
            {
                continue;
            }
            vk::WriteDescriptorSet descriptor_write;
            descriptor_write.dstSet          = descriptor_sets_[i];
            descriptor_write.dstBinding      = uniform.binding;
            descriptor_write.dstArrayElement = 0;
            descriptor_write.descriptorType  = GetVkDescriptorType(uniform.type);
            descriptor_write.descriptorCount = 1;
            if (uniform.type == EUniformDescriptorType::UniformBuffer || uniform.type == EUniformDescriptorType::DynamicUniformBuffer)
            {
                vk::DescriptorBufferInfo buffer_info;
                buffer_info.offset = uniform.offset;
                buffer_info.buffer = uniform_buffers_[uniform.name][i]->GetBufferHandle();
                if (uniform.type == EUniformDescriptorType::DynamicUniformBuffer)
                {
                    buffer_info.range = uniform.range;
                }
                else
                {
                    buffer_info.range = uniform.size;
                }
                buffer_infos.emplace_back(buffer_info);
                descriptor_write.pBufferInfo = &buffer_infos.back();
            }
            else
            {
                vk::DescriptorImageInfo image_info;
                image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                image_info.imageView   = default_lack_texture_view.GetHandle();
                image_info.sampler     = sampler.GetHandle();
                image_infos.emplace_back(image_info);
                descriptor_write.pImageInfo = &image_infos.back();
            }
            descriptor_writes.push_back(descriptor_write);
        }
        context.GetLogicalDevice()->UpdateDescriptorSets(descriptor_writes);
    }
}

void ShaderProgram::DestroyDescriptorSets()
{
    descriptor_sets_.clear();
    descriptor_set_names_.clear();
}

void ShaderProgram::CreateDescriptorPool()
{
    const auto&                                  device                = VulkanContext::Get()->GetLogicalDevice();
    core::StaticArray<vk::DescriptorPoolSize, 2> pool_sizes            = {};
    const auto                                   swapchain_image_count = g_engine_statistics.graphics.swapchain_image_count;

    // Uniform Object
    pool_sizes[0].type            = vk::DescriptorType::eUniformBuffer;
    pool_sizes[0].descriptorCount = swapchain_image_count;

    // 纹理采样器
    pool_sizes[1].type            = vk::DescriptorType::eCombinedImageSampler;
    pool_sizes[1].descriptorCount = swapchain_image_count;

    vk::DescriptorPoolCreateInfo pool_info = {};
    pool_info.setPoolSizes(pool_sizes).setMaxSets(swapchain_image_count);
    descriptor_pool_      = device->CreateDescriptorPool(pool_info);
    descriptor_pool_name_ = name_ + "_DescriptorPool";
    device->SetDescriptorPoolDebugName(descriptor_pool_, descriptor_pool_name_.Data());
}

void ShaderProgram::DestroyDescriptorPool()
{
    const auto& device = VulkanContext::Get()->GetLogicalDevice();
    device->DestroyDescriptorPool(descriptor_pool_);
    descriptor_pool_ = nullptr;
}

void ShaderProgram::CreateDescriptorSetLayout()
{
    const auto& device = VulkanContext::Get()->GetLogicalDevice();

    core::Array<vk::DescriptorSetLayoutBinding> uniform_bindings;
    for (const auto& uniform_binding: GetUniforms() | std::views::values)
    {
        vk::DescriptorSetLayoutBinding binding;
        binding.binding         = uniform_binding.binding;
        binding.descriptorType  = GetVkDescriptorType(uniform_binding.type);
        binding.descriptorCount = 1;
        binding.stageFlags      = GetVkShaderStage(uniform_binding.stage);
        uniform_bindings.push_back(binding);
    }
    vk::DescriptorSetLayoutCreateInfo layout_info{};
    layout_info.setBindings(uniform_bindings);
    descriptor_set_layout_ = device->CreateDescriptorSetLayout(layout_info);
    if (!name_.IsEmpty())
    {
        descriptor_set_layout_name_ = name_ + "_DescriptorSetLayout";
        device->SetDescriptionSetLayoutDebugName(descriptor_set_layout_, descriptor_set_layout_name_.Data());
    }
}

void ShaderProgram::DestroyDescriptorSetLayout()
{
    const auto& device = VulkanContext::Get()->GetLogicalDevice();

    device->DestroyDescriptorSetLayout(descriptor_set_layout_);
    descriptor_set_layout_ = nullptr;
}
}
