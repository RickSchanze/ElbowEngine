/**
 * @file VulkanMaterial.cpp
 * @author Echo
 * @Date 24-6-30
 * @brief
 */

#include "VulkanMaterial.h"

#include <ranges>

#include "RHI/Vulkan/VulkanContext.h"
#include "ShaderProgram.h"

RHI_VULKAN_NAMESPACE_BEGIN

void VulkanMaterial::CreateUniformBuffers()
{
    const auto&    device      = VulkanContext::Get().GetLogicalDevice();
    vk::DeviceSize buffer_size = 0;
    for (const auto& value: shader_program_->GetUniforms() | std::views::values)
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

void VulkanMaterial::CleanupUniformBuffers()
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

void VulkanMaterial::CreateDescriptorSets()
{
    VulkanContext&                context = VulkanContext::Get();
    TArray                        layouts(context.GetSwapChainImageCount(), descriptor_set_layout_);
    vk::DescriptorSetAllocateInfo alloc_info = {};
    alloc_info.setDescriptorPool(descriptor_pool_).setSetLayouts(layouts);
    descriptor_sets_.resize(context.GetSwapChainImageCount());
    // 描述符池对象销毁时会自动清除描述符集
    descriptor_sets_ = context.GetLogicalDevice()->AllocateDescriptorSets(alloc_info);
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

void VulkanMaterial::CleanupDescriptorSets()
{
    const auto& device = VulkanContext::Get().GetLogicalDevice();
    device->FreeDescriptorSets(descriptor_pool_, descriptor_sets_);
    descriptor_sets_.clear();
}

void VulkanMaterial::CreateDescriptorPool()
{
    const auto&                             device     = VulkanContext::Get().GetLogicalDevice();
    TStaticArray<vk::DescriptorPoolSize, 2> pool_sizes = {};
    const auto swapchain_image_count                   = g_engine_statistics.swapchain_image_count;

    // Uniform Object
    pool_sizes[0].type            = vk::DescriptorType::eUniformBuffer;
    pool_sizes[0].descriptorCount = swapchain_image_count;

    // 纹理采样器
    pool_sizes[1].type            = vk::DescriptorType::eCombinedImageSampler;
    pool_sizes[1].descriptorCount = swapchain_image_count;

    vk::DescriptorPoolCreateInfo PoolInfo = {};
    PoolInfo.setPoolSizes(pool_sizes).setMaxSets(swapchain_image_count);
    descriptor_pool_ = device->CreateDescriptorPool(PoolInfo);
}

void VulkanMaterial::CleanupDescriptorPool()
{
    const auto& device = VulkanContext::Get().GetLogicalDevice();
    device->DestroyDescriptorPool(descriptor_pool_);
    descriptor_pool_ = nullptr;
}

void VulkanMaterial::CreateDescriptorSetLayout()
{
    const auto& device = VulkanContext::Get().GetLogicalDevice();

    TArray<vk::DescriptorSetLayoutBinding> UniformBindings;
    for (const auto& uniform_binding: shader_program_->GetUniforms() | std::views::values)
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

void VulkanMaterial::CleanupDescriptorSetLayout()
{
    const auto& device = VulkanContext::Get().GetLogicalDevice();

    device->DestroyDescriptorSetLayout(descriptor_set_layout_);
    descriptor_set_layout_ = nullptr;
}

RHI_VULKAN_NAMESPACE_END
