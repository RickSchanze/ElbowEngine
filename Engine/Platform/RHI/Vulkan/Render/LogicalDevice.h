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
namespace rhi::vulkan
{
class SwapChain;
class PhysicalDevice;
class Instance;


class LogicalDevice final : public IRHIResource
{
public:
    ~LogicalDevice() override;

    Array<vk::DescriptorSet> AllocateDescriptorSets(const vk::DescriptorSetAllocateInfo& alloc_info) const;

    void FreeDescriptorSets(vk::DescriptorPool descriptor_pool, const Array<vk::DescriptorSet, std::allocator<vk::DescriptorSet>>& array) const;

    static UniquePtr<LogicalDevice> CreateUnique(vk::Device InDevice, const Ref<PhysicalDevice>& associated_physical_device);

    explicit LogicalDevice(ResourceProtected, vk::Device InDevice, const Ref<PhysicalDevice>& associated_physical_device);

    void DeInitialize();

    void Destroy() override;

    /**
     * 创建一个交换链对象
     * @param swap_chain_image_count 交换链图像数量 为0表示自动选择
     * @param width
     * @param height
     * @param log 是否打log
     * @return
     */
    UniquePtr<SwapChain> CreateSwapChain(uint32_t swap_chain_image_count = 0, int32_t width = 0, int32_t height = 0, bool log = true);

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

    vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& create_info, const AnsiString& debug_name = nullptr) const;

    vk::Queue GetGraphicsQueue() const { return graphics_queue_; }
    vk::Queue GetPresentQueue() const { return present_queue_; }

    void UpdateDescriptorSets(
        const vk::ArrayProxy<const vk::WriteDescriptorSet>& descriptor_writes,
        const vk::ArrayProxy<vk::CopyDescriptorSet>&        descriptor_copies = nullptr
    ) const;

    vk::Result MapMemory(vk::DeviceMemory InMemory, vk::DeviceSize InSize, vk::DeviceSize InOffset, void** OutData) const;

    void UnmapMemory(vk::DeviceMemory InMemory) const;

    void FlushMappedMemory(const Array<vk::MappedMemoryRange>& ranges) const;

    vk::Result WaitForFences(vk::ArrayProxy<vk::Fence> fences, bool wait_all = true, uint64_t timeout = UINT64_MAX) const;

    void ResetFences(vk::ArrayProxy<vk::Fence> fences) const;

    Array<vk::CommandBuffer> AllocateCommandBuffers(
        const vk::CommandBufferAllocateInfo& allocate_info, const char* debug_name = nullptr, Array<AnsiString>* out_debug_names = {}
    ) const;

    void DestroySampler(vk::Sampler sampler) const;

    void DestroySwapChain(vk::SwapchainKHR swap_chain) const;

    vk::Semaphore CreateDeviceSemaphore(const vk::SemaphoreCreateInfo& create_info) const;

    void DestroySemaphore(vk::Semaphore semaphore) const;

    vk::Fence CreateFence(const vk::FenceCreateInfo& create_info) const;

    void DestroyFence(vk::Fence fence) const;

    vk::RenderPass CreateRenderPass(const vk::RenderPassCreateInfo& create_info) const;
    void           DestroyRenderPass(vk::RenderPass render_pass) const;

    vk::CommandPool CreateCommandPool(const vk::CommandPoolCreateInfo& create_info, const char* debug_name = "") const;
    void            DestroyCommandPool(vk::CommandPool command_pool) const;

    vk::Pipeline
    CreateGraphicsPipeline(const vk::PipelineCache& cache, const vk::GraphicsPipelineCreateInfo& info, const AnsiString& debug_name = "") const;

    void SetObjectDebugName(const vk::DebugUtilsObjectNameInfoEXT& name_info) const;
    void SetCommandBufferDebugName(vk::CommandBuffer handle, const char* name) const;
    void SetImageDebugName(vk::Image handle, const char* name) const;
    void SetImageViewDebugName(vk::ImageView handle, const char* name) const;
    void SetRenderPassDebugName(vk::RenderPass handle, const char* name) const;
    void SetFramebufferDebugName(vk::Framebuffer handle, const char* name) const;
    void SetShaderModuleDebugName(vk::ShaderModule handle, const char* name) const;
    void SetBufferDebugName(vk::Buffer handle, const char* name) const;
    void SetBufferMemoryDebugName(vk::DeviceMemory handle, const char* name) const;
    void SetDescriptionSetLayoutDebugName(vk::DescriptorSetLayout handle, const char* name) const;
    void SetDescriptorSetDebugName(vk::DescriptorSet handle, const char* name) const;
    void SetDescriptorPoolDebugName(vk::DescriptorPool handle, const char* name) const;
    void SetPipelineDebugName(vk::Pipeline handle, const char* name) const;
    void SetPipelineLayoutDebugName(vk::PipelineLayout handle, const char* name) const;
    void SetDeviceMemoryDebugName(vk::DeviceMemory handle, const char* name) const;
    void SetCommandPoolDebugName(vk::CommandPool handle, const char* name) const;

    bool            IsValid() const { return static_cast<bool>(handle_); }
    vk::Device      GetHandle() const { return handle_; }
    PhysicalDevice& GetAssociatedPhysicalDevice() const { return associated_physical_device_; }

    void InitializePFNs();

private:
    vk::Device          handle_ = VK_NULL_HANDLE;
    Ref<PhysicalDevice> associated_physical_device_;

    vk::Queue graphics_queue_;
    vk::Queue present_queue_;

#if ELBOW_DEBUG
    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT_ = nullptr;
#endif
};
}
