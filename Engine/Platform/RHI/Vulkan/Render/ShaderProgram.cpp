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
#ifdef ELBOW_DEBUG
    debug_name_ = debug_name;
#endif
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

bool ShaderProgram::SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const
{
    const TStaticArray<glm::mat4, 3> ubo    = {model, view, projection};
    const LogicalDevice&             device = device_.get();
    // @TODO: 可能不需要每个都map memory
    for (int i = 0; i < g_engine_statistics.swapchain_image_count; i++)
    {
        void* data;
        auto  map_res = device.MapMemory(uniform_buffers_memory_[i], 3 * sizeof(glm::mat4), 0, &data);
        if (map_res != vk::Result::eSuccess)
        {
            LOG_ERROR_CATEGORY(Vulkan.Shader, L"设置Shader UBO: Map memory失败: {}", StringUtils::FromAnsiString(vk::to_string(map_res)));
            return false;
        }
        memcpy(data, ubo.data(), 3 * sizeof(glm::mat4));
        device.UnmapMemory(uniform_buffers_memory_[i]);
    }
    return true;
}

bool ShaderProgram::IsValid() const
{
    return descriptor_set_layout_ != nullptr && !descriptor_sets_.empty() && descriptor_pool_ != nullptr;
}

void ShaderProgram::CreateUniformBuffers()
{
    const auto&    device      = *VulkanContext::Get()->GetLogicalDevice();
    vk::DeviceSize buffer_size = 0;
    for (const auto& value: GetUniforms() | std::views::values)
    {
        buffer_size += value.size;
    }
    uniform_buffers_.resize(g_engine_statistics.swapchain_image_count);
    uniform_buffers_memory_.resize(g_engine_statistics.swapchain_image_count);
    uniform_buffer_debug_names_.resize(g_engine_statistics.swapchain_image_count);
    uniform_buffer_memory_debug_names_.resize(g_engine_statistics.swapchain_image_count);
    for (size_t i = 0; i < g_engine_statistics.swapchain_image_count; i++)
    {
        device.CreateBuffer(
            buffer_size,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            uniform_buffers_[i],
            uniform_buffers_memory_[i]
        );
#ifdef ELBOW_DEBUG
        if (!debug_name_.empty())
        {
            uniform_buffer_debug_names_[i] = debug_name_ + "_UniformBuffer_" + std::to_string(i);
            device.SetBufferDebugName(uniform_buffers_[i], uniform_buffer_debug_names_[i].data());
            uniform_buffer_memory_debug_names_[i] = debug_name_ + "_UniformBufferMemory_" + std::to_string(i);
            device.SetBufferMemoryDebugName(uniform_buffers_memory_[i], uniform_buffer_memory_debug_names_[i].data());
        }
#endif
    }
}

void ShaderProgram::DestroyUniformBuffers()
{
    const auto& device = VulkanContext::Get()->GetLogicalDevice();
    for (size_t i = 0; i < g_engine_statistics.swapchain_image_count; i++)
    {
        device->DestroyBuffer(uniform_buffers_[i]);
        device->FreeMemory(uniform_buffers_memory_[i]);
    }
    uniform_buffers_.clear();
    uniform_buffers_memory_.clear();
}

void ShaderProgram::CreateDescriptorSets()
{
    if (descriptor_set_layout_ == nullptr)
    {
        CreateDescriptorSetLayout();
    }
    VulkanContext&                context = *VulkanContext::Get();
    TArray                        layouts(g_engine_statistics.parallel_render_frame_count, descriptor_set_layout_);
    vk::DescriptorSetAllocateInfo alloc_info = {};
    alloc_info.setDescriptorPool(descriptor_pool_).setSetLayouts(layouts);
    descriptor_sets_.resize(context.GetSwapChainImageCount());
    // 描述符池对象销毁时会自动清除描述符集
    descriptor_sets_ = context.GetLogicalDevice()->AllocateDescriptorSets(alloc_info);
#ifdef ELBOW_DEBUG
    if (!debug_name_.empty())
    {
        for (size_t i = 0; i < descriptor_sets_.size(); i++)
        {
            debug_descriptor_set_names_.emplace_back(debug_name_ + "_DescriptorSet_" + std::to_string(i));
            context.GetLogicalDevice()->SetDescriptorSetDebugName(descriptor_sets_[i], debug_descriptor_set_names_.back().data());
        }
    }
#endif
    // 创建材质时首先使用默认丢失的贴图，之后需要调用更新贴图的方法
    const auto& default_lack_texture_view = Texture::GetDefaultLackTextureView();
    const auto& sampler                   = Sampler::GetDefaultSampler();
    for (size_t i = 0; i < descriptor_sets_.size(); i++)
    {
        TStaticArray<vk::WriteDescriptorSet, 1> descriptor_writes = {};

        vk::DescriptorBufferInfo buffer_info = {};
        buffer_info.setBuffer(uniform_buffers_[i]).setOffset(0).setRange(VK_WHOLE_SIZE);
        // TODO: DescriptorSet 应该动态更新而不是写死
        descriptor_writes[0].dstSet          = descriptor_sets_[i];
        descriptor_writes[0].dstBinding      = 0;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo     = &buffer_info;
        descriptor_writes[0].pImageInfo      = nullptr;

        vk::DescriptorImageInfo image_info = {};
        image_info.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(default_lack_texture_view.GetHandle())
            .setSampler(sampler.GetHandle());
        descriptor_writes[0].dstSet          = descriptor_sets_[i];
        descriptor_writes[0].dstBinding      = 1;
        descriptor_writes[0].dstArrayElement = 0;
        descriptor_writes[0].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
        descriptor_writes[0].descriptorCount = 1;
        descriptor_writes[0].pBufferInfo     = nullptr;
        descriptor_writes[0].pImageInfo      = &image_info;
        context.GetLogicalDevice()->UpdateDescriptorSets(descriptor_writes);
    }
}

void ShaderProgram::DestroyDescriptorSets()
{
    descriptor_sets_.clear();
#ifdef ELBOW_DEBUG
    debug_descriptor_set_names_.clear();
#endif
}

void ShaderProgram::CreateDescriptorPool()
{
    const auto&                             device                = VulkanContext::Get()->GetLogicalDevice();
    TStaticArray<vk::DescriptorPoolSize, 2> pool_sizes            = {};
    const auto                              swapchain_image_count = g_engine_statistics.swapchain_image_count;

    // Uniform Object
    pool_sizes[0].type            = vk::DescriptorType::eUniformBuffer;
    pool_sizes[0].descriptorCount = swapchain_image_count;

    // 纹理采样器
    pool_sizes[1].type            = vk::DescriptorType::eCombinedImageSampler;
    pool_sizes[1].descriptorCount = swapchain_image_count;

    vk::DescriptorPoolCreateInfo pool_info = {};
    pool_info.setPoolSizes(pool_sizes).setMaxSets(swapchain_image_count);
    descriptor_pool_ = device->CreateDescriptorPool(pool_info);
#ifdef ELBOW_DEBUG
    debug_descriptor_pool_name_ = debug_name_ + "_DescriptorPool";
    device->SetDescriptorPoolDebugName(descriptor_pool_, debug_descriptor_pool_name_.data());
#endif
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
#ifdef ELBOW_DEBUG
    if (!debug_name_.empty())
    {
        debug_descriptor_set_layout_name_ = debug_name_ + "_DescriptorSetLayout";
        device->SetDescriptionSetLayoutDebugName(descriptor_set_layout_, debug_descriptor_set_layout_name_.data());
    }
#endif
}

void ShaderProgram::DestroyDescriptorSetLayout()
{
    const auto& device = VulkanContext::Get()->GetLogicalDevice();

    device->DestroyDescriptorSetLayout(descriptor_set_layout_);
    descriptor_set_layout_ = nullptr;
}

RHI_VULKAN_NAMESPACE_END
