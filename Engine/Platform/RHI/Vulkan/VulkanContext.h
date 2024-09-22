/**
 * @file VulkanContext.h
 * @author Echo
 * @Date 24-4-23
 * @brief
 */

#pragma once
#include "Render/LogicalDevice.h"
#include "Render/SwapChain.h"
#include "RHI/GfxContext.h"
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


struct PreVulkanDeviceDestroyedSingnature : public TEvent<>
{
};

class IGraphicsPipeline;
// VulkanContext应该具有全局唯一单例
struct GraphicsQueueSubmitParams
{
    TArray<vk::Semaphore>          semaphores_to_wait;
    TArray<vk::Semaphore>          semaphores_to_singal;
    TArray<vk::PipelineStageFlags> wait_stages;
    bool                           has_self_semaphore = true;   // 本次图形管线提交需要生成一个由这次提交触发的信号量
};

class VulkanContext : public GfxContext
{
public:
    VulkanContext() = default;

    ~VulkanContext() override;

    bool CanRenderBackbuffer() const;

    static TUniquePtr<VulkanContext> CreateUnique(const TSharedPtr<Instance>& instance);

    PreVulkanDeviceDestroyedSingnature PreVulkanDeviceDestroyed;

protected:
    struct Protected
    {
    };

public:
    /** Begin GfxContext */

    EGraphicsAPI GetAPI() const override;

#ifdef ENABLE_PROFILING

    void InitProfiling() override;
    void DeInitProfiling() override;
    void BeginProfile(const char* name, const CommandBuffer& cmd) override;
    void EndProfile() override;
    void CollectProfileData(const CommandBuffer& cmd) override;

#endif

    /** End GfxContext */

    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanContext(Protected, const TSharedPtr<Instance>& instance);

    // 获取全局单例
    static VulkanContext* Get();

    void Initialize();
    void Finalize();

    vk::Semaphore SubmitGraphicsQueue(
        GraphicsQueueSubmitParams submit_params,
        // 提交参数
        vk::Fence                 fence_to_trigger   // 这次提交需要触发的fence
    );

    virtual void PrepareFrameRender();
    virtual void PostFrameRender();

    bool IsValid() const;

    static void RebuildSwapChain(int w, int h);

    // 获取交换链图像支持的格式
    vk::Format GetSwapChainImageFormat() const { return swap_chain_->GetImageFormat(); }

    // 获取深度图像支持的格式
    vk::Format GetDepthImageFormat() const;

    vk::Extent2D GetSwapChainExtent() const { return swap_chain_->GetExtent(); }

    int32_t GetSwapChainImageCount() const { return g_engine_statistics.graphics.swapchain_image_count; }

    TArray<TSharedPtr<ImageView>>& GetSwapChainImageViews() const { return swap_chain_->GetImageViews(); }

    ImageView* GetBackbufferView(int index) const { return back_buffer_views_[index]; }

    const TUniquePtr<CommandPool>& GetCommandPool() const { return command_pool_; }

    TUniquePtr<SwapChain>& GetSwapChain() { return swap_chain_; }

    TUniquePtr<CommandPool>& GetCommandPool() { return command_pool_; }

    TUniquePtr<LogicalDevice>& GetLogicalDevice() { return logical_device_; }

    TUniquePtr<PhysicalDevice>& GetPhysicalDevice() { return physical_device_; }

    TSharedPtr<Instance> GetVulkanInstance() { return vulkan_instance_; }

    vk::Semaphore GetRenderBeginWaitSemphore() const { return image_available_semaphores_[g_engine_statistics.current_frame_index]; }

    uint32_t GetMinUniformBufferOffsetAlignment() const;

    vk::CommandBuffer GetCurrentCommandBuffer() const;

    vk::CommandBuffer BeginRecordCommandBuffer();
    void              EndRecordCommandBuffer();

    int32_t GetActualBackbufferWidth() const { return back_buffers_[0]->GetWidth(); }
    int32_t GetActualBackbufferHeight() const { return back_buffers_[0]->GetHeight(); }

    /**
     * 现在能不能进行渲染
     * @return
     */
    bool CanRender() const;

    bool IsBackBufferValid() const;

protected:
    void CreateSyncObjects();
    void CleanSyncObjects() const;

    void CreateCommandBuffers();
    void DestroyCommandBuffers();

    static inline VulkanContext* s_context_ = nullptr;

    static void OnBackBufferResized(int w, int h);

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

    // 一次QueuePresent提交时需要等待的所有 Semaphore
    TArray<vk::Semaphore> all_wait_semaphores_;

    TArray<vk::CommandBuffer> command_buffers_;
    TArray<AnsiString>        command_buffers_names_;

    TArray<Image*>     back_buffers_;
    TArray<ImageView*> back_buffer_views_;

    bool wait_swapchain_rebuild_ = false;
};

RHI_VULKAN_NAMESPACE_END
