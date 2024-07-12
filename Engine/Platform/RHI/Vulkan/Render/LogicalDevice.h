/**
 * @file LogicalDevice.h
 * @author Echo 
 * @Date 24-4-21
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan
{
class SwapChain;
}
namespace RHI::Vulkan
{
class PhysicalDevice;
}
namespace RHI::Vulkan
{
class Instance;
}
RHI_VULKAN_NAMESPACE_BEGIN

class LogicalDevice final : public IRHIResource
{
public:
    ~LogicalDevice() override;

    TArray<vk::DescriptorSet> AllocateDescriptorSets(const vk::DescriptorSetAllocateInfo& alloc_info) const;

    void FreeDescriptorSets(vk::DescriptorPool descriptor_pool, const TArray<vk::DescriptorSet, std::allocator<vk::DescriptorSet>>& array) const;

    static TUniquePtr<LogicalDevice> CreateUnique(vk::Device InDevice, const Ref<PhysicalDevice>& InAssociatedPhysicalDevice);

    explicit LogicalDevice(ResourceProtected, vk::Device InDevice, const Ref<PhysicalDevice>& InAssociatedPhysicalDevice);

    void Finialize();

    void Destroy() override;

    /**
     * 创建一个交换链对象
     * @param InSwapChainImageCount 交换链图像数量 为0表示自动选择
     * @param InWidth
     * @param InHeight
     * @return
     */
    TUniquePtr<SwapChain> CreateSwapChain(uint32_t InSwapChainImageCount = 0, int32_t InWidth = 0, int32_t InHeight = 0);

    /**
     * 创建缓冲区 典型应用是辅助CPU加载数据和GPU读取数据
     * @param InSize
     * @param InUsage
     * @param InProperties
     * @param OutBuffer
     * @param OutBufferMemory
     */
    void CreateBuffer(
        vk::DeviceSize InSize, vk::BufferUsageFlags InUsage, vk::MemoryPropertyFlags InProperties, vk::Buffer& OutBuffer,
        vk::DeviceMemory& OutBufferMemory
    ) const;

    void DestroyBuffer(vk::Buffer buffer) const;

    void FreeMemory(vk::DeviceMemory memory) const;

    vk::DescriptorPool CreateDescriptorPool(const vk::DescriptorPoolCreateInfo& create_info) const;

    void DestroyDescriptorPool(vk::DescriptorPool pool) const;

    vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& create_info) const;

    void DestroyDescriptorSetLayout(vk::DescriptorSetLayout layout) const;

    void DestroyShaderModule(vk::ShaderModule module) const;

    vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& create_info) const;

    vk::Queue GetGraphicsQueue() const { return graphics_queue_; }
    vk::Queue GetPresentQueue() const { return present_queue_; }

    void UpdateDescriptorSets(
        const vk::ArrayProxy<const vk::WriteDescriptorSet>& descriptor_writes,
        const vk::ArrayProxy<vk::CopyDescriptorSet>&        descriptor_copies = nullptr
    ) const;

    vk::Result MapMemory(vk::DeviceMemory InMemory, vk::DeviceSize InSize, vk::DeviceSize InOffset, void** OutData) const;

    void UnmapMemory(vk::DeviceMemory InMemory) const;

    vk::Result WaitForFences(vk::ArrayProxy<vk::Fence> fences, bool wait_all = true, uint64_t timeout = UINT64_MAX) const;

    void ResetFences(vk::ArrayProxy<vk::Fence> fences) const;

    TArray<vk::CommandBuffer> AllocateCommandBuffers(const vk::CommandBufferAllocateInfo& allocate_info) const;

    void DestroySampler(vk::Sampler sampler) const;

    void DestroySwapChain(vk::SwapchainKHR swap_chain) const;

    vk::Semaphore CreateSemaphore(const vk::SemaphoreCreateInfo& create_info) const;

    void DestroySemaphore(vk::Semaphore semaphore) const;

    vk::Fence CreateFence(const vk::FenceCreateInfo& create_info) const;

    void DestroyFence(vk::Fence fence) const;

    bool            IsValid() const { return static_cast<bool>(handle_); }
    vk::Device      GetHandle() const { return handle_; }
    PhysicalDevice& GetAssociatedPhysicalDevice() const { return associated_physical_device_; }

private:
    vk::Device          handle_ = VK_NULL_HANDLE;
    Ref<PhysicalDevice> associated_physical_device_;

    vk::Queue graphics_queue_;
    vk::Queue present_queue_;
};

RHI_VULKAN_NAMESPACE_END
