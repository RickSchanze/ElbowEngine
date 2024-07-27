/**
 * @file VulkanContext.h
 * @author Echo
 * @Date 24-4-23
 * @brief
 */

#pragma once
#include "Interface/IGraphicsPipeline.h"
#include "Render/LogicalDevice.h"
#include "Render/SwapChain.h"
#include "Utils/ContainerUtils.h"
#include "VulkanCommon.h"


namespace RHI::Vulkan
{
class CommandPool;
class GraphicsPipeline;
}   // namespace RHI::Vulkan

namespace RHI::Vulkan
{
class RenderPass;
}

RHI_VULKAN_NAMESPACE_BEGIN
class IGraphicsPipeline;
// VulkanContext应该具有全局唯一单例
struct GraphicsQueueSubmitParams
{
    TArray<vk::Semaphore>          semaphores_to_wait;
    TArray<vk::Semaphore>          semaphores_to_singal;
    TArray<vk::PipelineStageFlags> wait_stages;
    bool                           has_self_semaphore = true;   // 本次图形管线提交需要生成一个由这次提交触发的信号量
};

class VulkanContext
{
public:
    VulkanContext() = default;

    virtual ~VulkanContext();

    static TUniquePtr<VulkanContext> CreateUnique(const TSharedPtr<Instance>& instance);

protected:
    struct Protected
    {
    };

public:
    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanContext(Protected, const TSharedPtr<Instance>& instance);

    // 获取全局单例
    static VulkanContext* Get();

    void Initialize();
    void Finalize();

    vk::Semaphore SubmitGraphicsQueue(
        const IGraphicsPipeline*  pipeline,
        // 管线
        GraphicsQueueSubmitParams submit_params,
        // 提交参数
        vk::Fence                 fence_to_trigger   // 这次提交需要触发的fence
    );

    virtual void PrepareFrameRender();
    virtual void PostFrameRender();

    bool IsValid() const;

    // 重建交换链
    void RebuildSwapChain();

    // 获取交换链图像支持的格式
    vk::Format GetSwapChainImageFormat() const { return swap_chain_->GetImageFormat(); }

    // 获取深度图像支持的格式
    vk::Format GetDepthImageFormat() const;

    vk::Extent2D GetSwapChainExtent() const { return swap_chain_->GetExtent(); }

    int32_t GetSwapChainImageCount() const { return g_engine_statistics.graphics.swapchain_image_count; }

    TArray<TSharedPtr<ImageView>>& GetSwapChainImageViews() const { return swap_chain_->GetImageViews(); }

    const TUniquePtr<CommandPool>& GetCommandPool() const { return command_pool_; }

    TUniquePtr<SwapChain>& GetSwapChain() { return swap_chain_; }

    TUniquePtr<CommandPool>& GetCommandPool() { return command_pool_; }

    TUniquePtr<LogicalDevice>& GetLogicalDevice() { return logical_device_; }

    TUniquePtr<PhysicalDevice>& GetPhysicalDevice() { return physical_device_; }

    TSharedPtr<Instance> GetVulkanInstance() { return vulkan_instance_; }

    vk::Semaphore GetRenderBeginWaitSemphore() const { return image_available_semaphores_[g_engine_statistics.current_frame_index]; }
    vk::Fence     GetInFlightFence() const { return in_flight_fences_[g_engine_statistics.current_frame_index]; }

    uint32_t GetMinUniformBufferOffsetAlignment() const;

protected:
    void CreateSyncObjecs();
    void CleanSyncObjects() const;

    static inline VulkanContext* s_context_ = nullptr;

private:
    int32_t renderer_id_ = 0;

    static inline int32_t s_renderer_id_count_ = 0;

    // 交换链
    TUniquePtr<SwapChain>      swap_chain_;
    // 游戏主线程的CommandPool
    TUniquePtr<CommandPool>    command_pool_;
    TUniquePtr<LogicalDevice>  logical_device_;
    TUniquePtr<PhysicalDevice> physical_device_;
    TSharedPtr<Instance>       vulkan_instance_;

    TArray<vk::Semaphore> image_available_semaphores_;
    TArray<vk::Fence>     in_flight_fences_;

    // 一次QueuePresent提交时需要等待的所有 Semaphore
    TArray<vk::Semaphore> all_wait_semaphores_;
};

RHI_VULKAN_NAMESPACE_END
