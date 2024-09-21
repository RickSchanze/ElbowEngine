/**
 * @file ShaderProgram.cpp
 * @author Echo 
 * @Date 24-4-29
 * @brief 
 */

#include "ShaderProgram.h"

#include "CoreEvents.h"
#include "CoreGlobal.h"
#include "Misc/Vertex.h"
#include "Utils/StringUtils.h"

#include <ranges>

#include "RHI/Vulkan/Resource/Buffer.h"

RHI_VULKAN_NAMESPACE_BEGIN

class ShaderProgramManager
{
    struct ShaderPath
    {
        Path vert;
        Path frag;
    };

    struct ShaderPathHash
    {
        size_t operator()(const ShaderPath& p) const
        {
            return std::hash<String>()(p.vert.ToRelativeString()) ^ std::hash<String>()(p.frag.ToRelativeString());
        }
    };

    struct ShaderPathEqual
    {
        bool operator()(const ShaderPath& p1, const ShaderPath& p2) const { return p1.vert == p2.vert && p1.frag == p2.frag; }
    };

public:
    static ShaderProgram* Request(const Path& v, const Path& f)
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
            VulkanContext::Get()->PreVulkanDeviceDestroyed.Add(&ShaderProgramManager::DestroyAll);
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
    inline static THashMap<ShaderPath, ShaderProgram*, ShaderPathHash, ShaderPathEqual> shader_programs_;
};

ShaderProgram::ShaderProgram(
    const Ref<LogicalDevice> device, Shader* vert, Shader* frag, const EShaderDestroyTime destroy_time, const AnsiString& debug_name
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

ShaderProgram* ShaderProgram::Create(Shader* vert, Shader* frag, const EShaderDestroyTime destroy_time, const AnsiString& debug_name)
{
    Ref   device = *VulkanContext::Get()->GetLogicalDevice();
    auto* prog   = ShaderProgramManager::Request(vert->GetShaderPath(), frag->GetShaderPath());
    if (prog != nullptr)
    {
        return prog;
    }
    prog = New<ShaderProgram>(device, vert, frag, destroy_time, debug_name);
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
                LOG_ERROR_CATEGORY(Vulkan, L"Uniform变量Binding不一致: Name: {}", StringUtils::FromAnsiString(uniform.name));
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

TArray<vk::VertexInputAttributeDescription> ShaderProgram::GetVertexInputAttributeDescriptions() const
{
    TArray<vk::VertexInputAttributeDescription> AttributeDesc;
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

TArray<vk::VertexInputBindingDescription> ShaderProgram::GetVertexInputBindingDescription() const
{
    TArray<vk::VertexInputBindingDescription> BindingDescs;
    vk::VertexInputBindingDescription         Desc{};
    // clang-format off
    Desc
        .setBinding(0)
        .setStride(GetStride())
        .setInputRate(vk::VertexInputRate::eVertex);
    // clang-format on
    BindingDescs.push_back(Desc);
    return BindingDescs;
}

uint32_t ShaderProgram::GetStride() const
{
    // Mesh里Vertex的大小
    return sizeof(Vertex);
}

bool ShaderProgram::SetTexture(const AnsiString& name, const ImageView& view, const Sampler& sampler)
{
    if (!uniforms_.contains(name))
    {
        return false;
    }
    if (!view.IsValid())
    {
        LOG_ERROR_ANSI_CATEGORY(Vulkan, "ShaderProgram::SetTexture: ImageView {} is invalid in ShaderProgram {}", name, name_);
        return false;
    }
    if (!sampler.IsValid())
    {
        LOG_ERROR_ANSI_CATEGORY(Vulkan, "ShaderProgram::SetTexture: Sampler {} is invalid in ShaderProgram {}", name, name_);
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

void ShaderProgram::SetUniformBuffer(const AnsiString& name, const void* data, size_t size)
{
    if (!uniform_buffers_.contains(name))
    {
        LOG_ERROR_ANSI_CATEGORY(Vulkan, "ShaderProgram::SetStaticUniformBuffer: UniformBuffer {} not found", name);
        return;
    }
    auto& buffers = uniform_buffers_[name];
    for (size_t i = 0; i < g_engine_statistics.graphics.parallel_render_frame_count; i++)
    {
        auto& buffer = buffers[i];
        buffer->MapMemory();
        buffer->GetMappedCpuMemory();
        buffer->Memcpy(data, size);
        buffer->FlushMemory();
    }
}

bool ShaderProgram::HasShaderUniform(const AnsiString& name) const
{
    return uniforms_.contains(name);
}

const TArray<PushConstantDescriptor>& ShaderProgram::GetVertexPushConstants() const
{
    return vert_shader_->GetPushConstantDescriptors();
}

const TArray<PushConstantDescriptor>& ShaderProgram::GetFragmentPushConstants() const
{
    return frag_shader_->GetPushConstantDescriptors();
}

bool ShaderProgram::SetCubeTexture(const AnsiString& name, const ImageView& image_view, const Sampler& sampler)
{
    if (!uniforms_.contains(name))
    {
        LOG_ERROR_ANSI_CATEGORY(Vulkan.ShaderProgram, "Not find cubemap sampler {} in ShaderProgram {}", name, name_);
        return false;
    }
    if (!sampler.IsValid())
    {
        LOG_ERROR_ANSI_CATEGORY(Vulkan.ShaderProgram, "Sampler {} is invalid when update cubemap sampler in ShaderProgram {}", name, name_);
        return false;
    }
    if (!image_view.IsValid())
    {
        LOG_ERROR_ANSI_CATEGORY(Vulkan.ShaderProgram, "ImageView {} is invalid when update cubemap sampler in ShaderProgram {}", name, name_);
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
    TArray<vk::WriteDescriptorSet> write_descriptor_sets;
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
            TArray<Buffer*> new_buffer(g_engine_statistics.graphics.parallel_render_frame_count);
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
    TArray                        layouts(g_engine_statistics.graphics.parallel_render_frame_count, descriptor_set_layout_);
    vk::DescriptorSetAllocateInfo alloc_info = {};
    alloc_info.setDescriptorPool(descriptor_pool_).setSetLayouts(layouts);
    // 描述符池对象销毁时会自动清除描述符集
    descriptor_sets_ = context.GetLogicalDevice()->AllocateDescriptorSets(alloc_info);

    if (!name_.empty())
    {
        for (size_t i = 0; i < descriptor_sets_.size(); i++)
        {
            descriptor_set_names_.emplace_back(name_ + "_DescriptorSet_" + std::to_string(i));
            context.GetLogicalDevice()->SetDescriptorSetDebugName(descriptor_sets_[i], descriptor_set_names_.back().data());
        }
    }

    // 创建材质时首先使用默认丢失的贴图，之后需要调用更新贴图的方法
    const auto& default_lack_texture_view = Texture::GetDefaultLackTextureView();
    const auto& sampler                   = Sampler::GetDefaultSampler();
    for (size_t i = 0; i < descriptor_sets_.size(); i++)
    {
        TArray<vk::WriteDescriptorSet>   descriptor_writes = {};
        TArray<vk::DescriptorBufferInfo> buffer_infos;
        TArray<vk::DescriptorImageInfo>  image_infos;

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
    const auto&                             device                = VulkanContext::Get()->GetLogicalDevice();
    TStaticArray<vk::DescriptorPoolSize, 2> pool_sizes            = {};
    const auto                              swapchain_image_count = g_engine_statistics.graphics.swapchain_image_count;

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
    device->SetDescriptorPoolDebugName(descriptor_pool_, descriptor_pool_name_.data());
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

    TArray<vk::DescriptorSetLayoutBinding> uniform_bindings;
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
    if (!name_.empty())
    {
        descriptor_set_layout_name_ = name_ + "_DescriptorSetLayout";
        device->SetDescriptionSetLayoutDebugName(descriptor_set_layout_, descriptor_set_layout_name_.data());
    }
}

void ShaderProgram::DestroyDescriptorSetLayout()
{
    const auto& device = VulkanContext::Get()->GetLogicalDevice();

    device->DestroyDescriptorSetLayout(descriptor_set_layout_);
    descriptor_set_layout_ = nullptr;
}

RHI_VULKAN_NAMESPACE_END
