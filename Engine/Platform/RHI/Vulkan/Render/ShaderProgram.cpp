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
    const Ref<LogicalDevice> device, Shader* vert, Shader* frag, const EShaderDestroyTime destroy_time
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
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();
}

ShaderProgram::~ShaderProgram()
{
    DestroyShaders();
    DestroyDescriptorSets();
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
                LOG_ERROR_CATEGORY(
                    Vulkan, L"Uniform变量Binding不一致: Name: {}", StringUtils::FromAnsiString(uniform.name)
                );
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

UInt32 ShaderProgram::GetStride() const
{
    UInt32 Stride = 0;
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

bool ShaderProgram::SetUniformBufferObject(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
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
            LOG_ERROR_CATEGORY(
                Vulkan.Shader,
                L"设置Shader UBO: Map memory失败: {}",
                StringUtils::FromAnsiString(vk::to_string(map_res))
            );
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
    const auto&    device      = VulkanContext::Get().GetLogicalDevice();
    vk::DeviceSize buffer_size = 0;
    for (const auto& value: GetUniforms() | std::views::values)
    {
        buffer_size += value.size;
    }
    uniform_buffers_.resize(g_engine_statistics.swapchain_image_count);
    uniform_buffers_memory_.resize(g_engine_statistics.swapchain_image_count);
    for (size_t i = 0; i < g_engine_statistics.swapchain_image_count; i++)
    {
        device->CreateBuffer(
            buffer_size,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            uniform_buffers_[i],
            uniform_buffers_memory_[i]
        );
    }
}

void ShaderProgram::DestroyUniformBuffers()
{
    const auto& device = VulkanContext::Get().GetLogicalDevice();
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
    VulkanContext&                context = VulkanContext::Get();
    TArray                        layouts(context.GetSwapChainImageCount(), descriptor_set_layout_);
    vk::DescriptorSetAllocateInfo alloc_info = {};
    alloc_info.setDescriptorPool(descriptor_pool_).setSetLayouts(layouts);
    descriptor_sets_.resize(context.GetSwapChainImageCount());
    // 描述符池对象销毁时会自动清除描述符集
    descriptor_sets_                      = context.GetLogicalDevice()->AllocateDescriptorSets(alloc_info);
    // 创建材质时首先使用默认丢失的贴图，之后需要调用更新贴图的方法
    const auto& default_lack_texture_view = Texture::GetDefaultLackTextureView();
    const auto& sampler                   = Sampler::GetDefaultSampler();
    for (size_t i = 0; i < descriptor_sets_.size(); i++)
    {
        vk::DescriptorBufferInfo buffer_info = {};
        buffer_info.setBuffer(uniform_buffers_[i]).setOffset(0).setRange(VK_WHOLE_SIZE);
        vk::DescriptorImageInfo image_info = {};
        image_info.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(default_lack_texture_view.GetHandle())
            .setSampler(sampler.GetHandle());

        TStaticArray<vk::WriteDescriptorSet, 2> descriptor_writes = {};

        // TODO: DescriptorSet 应该动态更新而不是写死
        vk::WriteDescriptorSet descriptor_write = {};
        descriptor_write.setDstSet(descriptor_sets_[i])
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setPBufferInfo(&buffer_info);
        descriptor_writes[0]                 = descriptor_write;
        descriptor_writes[1].dstSet          = descriptor_sets_[i];
        descriptor_writes[1].dstBinding      = 1;
        descriptor_writes[1].dstArrayElement = 0;
        descriptor_writes[1].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
        descriptor_writes[1].descriptorCount = 1;
        descriptor_writes[1].pImageInfo      = &image_info;
        context.GetLogicalDevice()->UpdateDescriptorSets(descriptor_writes);
    }
}

void ShaderProgram::DestroyDescriptorSets()
{
    const auto& device = VulkanContext::Get().GetLogicalDevice();
    device->FreeDescriptorSets(descriptor_pool_, descriptor_sets_);
    descriptor_sets_.clear();
}

void ShaderProgram::CreateDescriptorPool()
{
    const auto&                             device                = VulkanContext::Get().GetLogicalDevice();
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
}

void ShaderProgram::DestroyDescriptorPool()
{
    const auto& device = VulkanContext::Get().GetLogicalDevice();
    device->DestroyDescriptorPool(descriptor_pool_);
    descriptor_pool_ = nullptr;
}

void ShaderProgram::CreateDescriptorSetLayout()
{
    const auto& device = VulkanContext::Get().GetLogicalDevice();

    TArray<vk::DescriptorSetLayoutBinding> UniformBindings;
    for (const auto& uniform_binding: GetUniforms() | std::views::values)
    {
        vk::DescriptorSetLayoutBinding binding;
        binding.binding         = uniform_binding.binding;
        binding.descriptorType  = GetVkDescriptorType(uniform_binding.type);
        binding.descriptorCount = 1;
        binding.stageFlags      = GetVkShaderStage(uniform_binding.stage);
        UniformBindings.push_back(binding);
    }
    vk::DescriptorSetLayoutCreateInfo LayoutInfo{};
    LayoutInfo.setBindings(UniformBindings);
    descriptor_set_layout_ = device->CreateDescriptorSetLayout(LayoutInfo);
}

void ShaderProgram::DestroyDescriptorSetLayout()
{
    const auto& device = VulkanContext::Get().GetLogicalDevice();

    device->DestroyDescriptorSetLayout(descriptor_set_layout_);
    descriptor_set_layout_ = nullptr;
}

RHI_VULKAN_NAMESPACE_END
