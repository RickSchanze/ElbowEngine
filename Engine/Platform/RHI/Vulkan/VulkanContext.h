/**
 * @file VulkanContext.h
 * @author Echo
 * @Date 24-4-23
 * @brief
 */

#pragma once
#include "Render/LogicalDevice.h"
#include "Render/RenderPass.h"
#include "Render/SwapChain.h"
#include "Utils/ContainerUtils.h"
#include "VulkanCommon.h"

class IGraphicsPipeline;
namespace RHI::Vulkan
{
class CommandPool;
class GraphicsPipeline;
} // namespace RHI::Vulkan

namespace RHI::Vulkan
{
class RenderPass;
}

RHI_VULKAN_NAMESPACE_BEGIN

// VulkanContext应该具有全局唯一单例
class VulkanContext
{
public:
    VulkanContext() = default;

    virtual ~VulkanContext();

    static TUniquePtr<VulkanContext> CreateUnique(const TSharedPtr<Instance> &InVulkanInstance);

protected:
    struct Protected
    {
    };

public:
    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanContext(Protected, const TSharedPtr<Instance> &InVulkanInstance);

    // 获取全局单例
    static VulkanContext &Get();

    void Initialize();
    void Finalize();

    void Draw();

    bool IsValid() const;

    // 重建交换链
    void RebuildSwapChain();

    // 获取交换链图像支持的格式
    vk::Format GetSwapChainImageFormat() const
    {
        return swap_chain_->GetImageFormat();
    }

    // 获取深度图像支持的格式
    vk::Format GetDepthImageFormat();

    vk::Extent2D GetSwapChainExtent() const
    {
        return swap_chain_->GetExtent();
    }

    UInt32 GetSwapChainImageCount() const
    {
        return swap_chain_image_count_;
    }

    TArray<TSharedPtr<ImageView>> &GetSwapChainImageViews() const
    {
        return swap_chain_->GetImageViews();
    }

    const TUniquePtr<CommandPool> &GetCommandPool() const
    {
        return command_pool_;
    }

    virtual void CreateGraphicsPipeline();

    // 向渲染器提交命令缓冲区
    void AddPipelineToRender(IGraphicsPipeline *InPipeline)
    {
        render_graphics_pipelines_.push_back(InPipeline);
    }

    void RemovePipelineFromRender(IGraphicsPipeline *InPipeline)
    {
        ContainerUtils::Remove(render_graphics_pipelines_, InPipeline);
    }

    TUniquePtr<SwapChain> &GetSwapChain()
    {
        return swap_chain_;
    }
    TUniquePtr<CommandPool> &GetCommandPool()
    {
        return command_pool_;
    }
    TUniquePtr<LogicalDevice> &GetLogicalDevice()
    {
        return logical_device_;
    }
    TUniquePtr<PhysicalDevice> &GetPhysicalDevice()
    {
        return physical_device_;
    }
    TSharedPtr<Instance> GetVulkanInstance()
    {
        return vulkan_instance_;
    }

protected:
    void CreateSyncObjecs();
    void CleanSyncObjects();

    static inline VulkanContext *sContext = nullptr;

private:
    Int32 swap_chain_image_count_ = 3;

    Int32 renderer_id_ = 0;

    static inline Int32 s_renderer_id_count_ = 0;

    // TODO: 图形管线和Shader一起属于材质系统
    GraphicsPipeline *graphics_pipeline_ = nullptr;

    // 交换链
    TUniquePtr<SwapChain> swap_chain_;
    // 游戏主线程的CommandPool
    TUniquePtr<CommandPool> command_pool_;
    TUniquePtr<LogicalDevice> logical_device_;
    TUniquePtr<PhysicalDevice> physical_device_;
    TSharedPtr<Instance> vulkan_instance_;

    TArray<vk::Semaphore> image_available_semaphores_;
    TArray<vk::Semaphore> image_render_finished_semaphores_;
    TArray<vk::Fence> in_flight_fences_;

    TArray<IGraphicsPipeline*> render_graphics_pipelines_;
};

RHI_VULKAN_NAMESPACE_END
