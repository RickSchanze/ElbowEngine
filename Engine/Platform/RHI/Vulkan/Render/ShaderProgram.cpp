/**
 * @file ShaderProgram.cpp
 * @author Echo 
 * @Date 24-4-29
 * @brief 
 */

#include "ShaderProgram.h"

#include "CoreGlobal.h"
#include "Utils/StringUtils.h"

#include <ranges>

#include "glm/matrix.hpp"
#include "RHI/Vulkan/Resource/Buffer.h"
#include "RHI/Vulkan/VulkanUtils.h"

RHI_VULKAN_NAMESPACE_BEGIN

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
    DestroyShaders();
    DestroyDescriptorSets();
    DestroyDescriptorSetLayout();
    DestroyDescriptorPool();
    DestroyUniformBuffers();
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
    uint32_t Stride = 0;
    for (const auto& Attribute: vertex_input_attributes_)
    {
        Stride += Attribute.size;
    }
    return Stride;
}

void ShaderProgram::DestroyShaders()
{
    if (vert_shader_)
    {
        delete vert_shader_;
        vert_shader_ = nullptr;
    }
    if (frag_shader_)
    {
        delete frag_shader_;
        frag_shader_ = nullptr;
    }
}

void ShaderProgram::SetVP(const glm::mat4& view, const glm::mat4& projection) const
{
    const TStaticArray<glm::mat4, 2> ubo = {projection, view};

    auto* current_buffer = static_vp_uniform_buffers_[g_engine_statistics.current_frame_index];
    current_buffer->MapMemory();
    auto* map_res = static_vp_uniform_buffers_[g_engine_statistics.current_frame_index]->GetMappedCpuMemory();
    memcpy(map_res, ubo.data(), 2 * sizeof(glm::mat4));
    current_buffer->UnmapMemory();
}

void ShaderProgram::SetM(glm::mat4* models, size_t size) const
{
    auto* current_buffer = dynamic_model_uniform_buffers_[g_engine_statistics.current_frame_index];
    if (!current_buffer->IsMemoryMapped())
    {
        current_buffer->MapMemory();
    }
    current_buffer->Memcpy(models, size);
    current_buffer->FlushMemory();
}

bool ShaderProgram::SetTexture(const AnsiString& name, const ImageView& view, const Sampler& sampler)
{
    if (!uniforms_.contains(name))
    {
        return false;
    }
    vk::WriteDescriptorSet descriptor_write;
    const auto& tex_uniform = uniforms_[name];
    descriptor_write.dstBinding = tex_uniform.binding;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    descriptor_write.descriptorCount = 1;
    vk::DescriptorImageInfo image_info;
    image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    image_info.imageView = view.GetHandle();
    image_info.sampler = sampler.GetHandle();
    for (const auto descriptor_set : descriptor_sets_)
    {
        descriptor_write.dstSet = descriptor_set;
        descriptor_write.pImageInfo = &image_info;
        VulkanContext::Get()->GetLogicalDevice()->UpdateDescriptorSets(descriptor_write);
    }
    return true;
}

bool ShaderProgram::IsValid() const
{
    return descriptor_set_layout_ != nullptr && !descriptor_sets_.empty() && descriptor_pool_ != nullptr;
}

void ShaderProgram::CreateUniformBuffers()
{
    vk::DeviceSize buffer_size = 0;

    // 静态共享的VP矩阵的UniformBuffer
    for (const auto& value: GetUniforms() | std::views::values)
    {
        if (value.name != "ubo_instance")
        {
            buffer_size += value.size;
        }
    }
    static_vp_uniform_buffers_.resize(g_engine_statistics.graphics.parallel_render_frame_count);

    for (size_t i = 0; i < g_engine_statistics.graphics.parallel_render_frame_count; i++)
    {
        static_vp_uniform_buffers_[i] = new Buffer(
            buffer_size,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            name_ + "_StaticSharedUniformBuffer"
        );
    }
    size_t dynamic_alignment = VulkanUtils::GetDynamicUniformModelAligment();
    // 动态实例的模型变换矩阵的uniform buffer
    buffer_size              = dynamic_alignment * g_engine_statistics.graphics.max_dynamic_model_uniform_buffer_count;
    dynamic_model_uniform_buffers_.resize(g_engine_statistics.graphics.parallel_render_frame_count);
    for (size_t i = 0; i < g_engine_statistics.graphics.parallel_render_frame_count; i++)
    {
        dynamic_model_uniform_buffers_[i] = new Buffer(
            buffer_size, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible, name_ + "_DynamiModelUniformBuffer"
        );
    }
}

void ShaderProgram::DestroyUniformBuffers()
{
    for (size_t i = 0; i < g_engine_statistics.graphics.parallel_render_frame_count; i++)
    {
        delete static_vp_uniform_buffers_[i];
        delete dynamic_model_uniform_buffers_[i];
    }
    static_vp_uniform_buffers_.clear();
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

        buffer_infos.reserve(4);

        for (auto& uniform: uniforms_ | std::views::values)
        {
            vk::WriteDescriptorSet descriptor_write;
            descriptor_write.dstSet          = descriptor_sets_[i];
            descriptor_write.dstBinding      = uniform.binding;
            descriptor_write.dstArrayElement = 0;
            descriptor_write.descriptorType  = GetVkDescriptorType(uniform.type);
            descriptor_write.descriptorCount = 1;
            if (uniform.type == EUniformDescriptorType::UniformBuffer || uniform.type == EUniformDescriptorType::DynamicUniformBuffer)
            {
                vk::DescriptorBufferInfo buffer_info;
                if (uniform.type == EUniformDescriptorType::UniformBuffer)
                {
                    buffer_info.buffer = static_vp_uniform_buffers_[i]->GetBufferHandle();
                }
                else if (uniform.type == EUniformDescriptorType::DynamicUniformBuffer)
                {
                    buffer_info.buffer = dynamic_model_uniform_buffers_[i]->GetBufferHandle();
                }
                buffer_info.offset = uniform.offset;
                buffer_info.range  = uniform.size;
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
