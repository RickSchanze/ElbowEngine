/**
 * @file VulkanContext.h
 * @author Echo
 * @Date 24-4-23
 * @brief
 */

#pragma once
#include "Core/Event/Event.h"
#include "Render/LogicalDevice.h"
#include "Render/SwapChain.h"
#include "Platform/RHI/GfxContext.h"
#include "Core/Utils/ContainerUtils.h"
#include "vulkan/vulkan.hpp"


namespace rhi::vulkan
{
class CommandPool;
class GraphicsPipeline;
class RenderPass;

DECLARE_MULTICAST_EVENT(PreVulkanDeviceDestroyedEvent);

class IGraphicsPipeline;
// VulkanContext应该具有全局唯一单例
struct GraphicsQueueSubmitParams
{
    core::Array<vk::Semaphore>          semaphores_to_wait;
    core::Array<vk::Semaphore>          semaphores_to_signal;
    core::Array<vk::PipelineStageFlags> wait_stages;
    bool                                has_self_semaphore = true;   // 本次图形管线提交需要生成一个由这次提交触发的信号量
};

class VulkanContext : public GfxContext
{
public:
    VulkanContext() = default;

    ~VulkanContext() override;

    [[nodiscard]] bool CanRenderBackbuffer() const;

    static core::UniquePtr<VulkanContext> CreateUnique(const core::SharedPtr<Instance>& instance);

    PreVulkanDeviceDestroyedEvent OnPreVulkanDeviceDestroyed;

protected:
    struct Protected
    {
    };

public:
    /** Begin GfxContext */

    [[nodiscard]] EGraphicsAPI GetAPI() const override;


    void InitProfiling() override;
    void DeInitProfiling() override;
    void BeginProfile(const char* name, const CommandBuffer& cmd) override;
    void EndProfile() override;
    void CollectProfileData(const CommandBuffer& cmd) override;


    /** End GfxContext */

    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanContext(Protected, const core::SharedPtr<Instance>& instance);

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

    [[nodiscard]] bool IsValid() const;

    static void RebuildSwapChain(int w, int h);

    // 获取交换链图像支持的格式
    [[nodiscard]] vk::Format GetSwapChainImageFormat() const { return swap_chain_->GetImageFormat(); }

    // 获取深度图像支持的格式
    [[nodiscard]] vk::Format GetDepthImageFormat() const;

    [[nodiscard]] vk::Extent2D GetSwapChainExtent() const { return swap_chain_->GetExtent(); }

    [[nodiscard]] int32_t GetSwapChainImageCount() const { return g_engine_statistics.graphics.swapchain_image_count; }

    [[nodiscard]] core::Array<core::SharedPtr<ImageView>>& GetSwapChainImageViews() const { return swap_chain_->GetImageViews(); }

    [[nodiscard]] ImageView* GetBackbufferView(int index) const { return back_buffer_views_[index]; }

    [[nodiscard]] const core::UniquePtr<CommandPool>& GetCommandPool() const { return command_pool_; }

    core::UniquePtr<SwapChain>& GetSwapChain() { return swap_chain_; }

    core::UniquePtr<CommandPool>& GetCommandPool() { return command_pool_; }

    core::UniquePtr<LogicalDevice>& GetLogicalDevice() { return logical_device_; }

    core::UniquePtr<PhysicalDevice>& GetPhysicalDevice() { return physical_device_; }

    core::SharedPtr<Instance> GetVulkanInstance() { return vulkan_instance_; }

    [[nodiscard]] vk::Semaphore GetRenderBeginWaitSemphore() const { return image_available_semaphores_[g_engine_statistics.current_frame_index]; }

    [[nodiscard]] uint32_t GetMinUniformBufferOffsetAlignment() const;

    auto GetCurrentCommandBuffer() const-> vk::CommandBuffer;

    vk::CommandBuffer BeginRecordCommandBuffer();
    void              EndRecordCommandBuffer();

    [[nodiscard]] int32_t GetActualBackbufferWidth() const { return back_buffers_[0]->GetWidth(); }
    [[nodiscard]] int32_t GetActualBackbufferHeight() const { return back_buffers_[0]->GetHeight(); }

    /**
     * 现在能不能进行渲染
     * @return
     */
    [[nodiscard]] bool CanRender() const;

    [[nodiscard]] bool IsBackBufferValid() const;

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
    core::UniquePtr<SwapChain>      swap_chain_;
    // 游戏主线程的CommandPool
    core::UniquePtr<CommandPool>    command_pool_;
    core::UniquePtr<LogicalDevice>  logical_device_;
    core::UniquePtr<PhysicalDevice> physical_device_;
    core::SharedPtr<Instance>       vulkan_instance_;

    core::Array<vk::Semaphore> image_available_semaphores_;

    // 一次QueuePresent提交时需要等待的所有 Semaphore
    core::Array<vk::Semaphore> all_wait_semaphores_;

    core::Array<vk::CommandBuffer> command_buffers_;
    core::Array<core::String>      command_buffers_names_;

    core::Array<Image*>     back_buffers_;
    core::Array<ImageView*> back_buffer_views_;

    bool wait_swapchain_rebuild_ = false;
};
}
