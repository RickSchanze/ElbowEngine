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
namespace RHI::Vulkan {
class CommandPool;
class GraphicsPipeline;
}   // namespace RHI::Vulkan

namespace RHI::Vulkan {
class RenderPass;
}

RHI_VULKAN_NAMESPACE_BEGIN

// VulkanContext应该具有全局唯一单例
class VulkanContext {
public:
    VulkanContext() = default;

    virtual ~VulkanContext();

    static TUniquePtr<VulkanContext> CreateUnique(const TSharedPtr<Instance>& InVulkanInstance);

protected:
    struct Protected
    {};

public:
    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanContext(Protected, const TSharedPtr<Instance>& InVulkanInstance);

    // 获取全局单例
    static VulkanContext& Get();

    void Initialize();
    void Finalize();

    void Draw();

    bool IsValid() const;

    // 重建交换链
    void RebuildSwapChain();

    // 获取交换链图像支持的格式
    vk::Format GetSwapChainImageFormat() const { return mSwapChain->GetImageFormat(); }

    // 获取深度图像支持的格式
    vk::Format GetDepthImageFormat();

    vk::Extent2D GetSwapChainExtent() const { return mSwapChain->GetExtent(); }

    UInt32 GetSwapChainImageCount() const { return mSwapChainImageCount; }

    TArray<TSharedPtr<ImageView>>& GetSwapChainImageViews() const { return mSwapChain->GetImageViews(); }

    const TUniquePtr<CommandPool>& GetCommandPool() const { return mCommandPool; }

    virtual void CreateGraphicsPipeline();

    // 向渲染器提交命令缓冲区
    void AddPipelineToRender(IGraphicsPipeline* InPipeline) { mRenderGraphicsPipelines.push_back(InPipeline); }
    void RemovePipelineFromRender(IGraphicsPipeline* InPipeline) { ContainerUtils::Remove(mRenderGraphicsPipelines, InPipeline); }

    TUniquePtr<SwapChain>&      GetSwapChain() { return mSwapChain; }
    TUniquePtr<CommandPool>&    GetCommandPool() { return mCommandPool; }
    TUniquePtr<LogicalDevice>&  GetLogicalDevice() { return mLogicalDevice; }
    TUniquePtr<PhysicalDevice>& GetPhysicalDevice() { return mPhysicalDevice; }
    TSharedPtr<Instance>        GetVulkanInstance() { return mVulkanInstance; }

protected:
    void CreateSyncObjecs();
    void CleanSyncObjects();

    static inline VulkanContext* sContext = nullptr;

private:
    Int32 mSwapChainImageCount = 3;

    int   mCurrentFrame      = 0;

    Int32 mRendererID = 0;

    static inline Int32 sRendererIDCount = 0;

    // TODO: 图形管线和Shader一起属于材质系统
    GraphicsPipeline* mGraphicsPipeline = nullptr;

    // 交换链
    TUniquePtr<SwapChain>      mSwapChain;
    // 游戏主线程的CommandPool
    TUniquePtr<CommandPool>    mCommandPool;
    TUniquePtr<LogicalDevice>  mLogicalDevice;
    TUniquePtr<PhysicalDevice> mPhysicalDevice;
    TSharedPtr<Instance>       mVulkanInstance;

    TArray<vk::Semaphore> mImageAvailableSemaphores;
    TArray<vk::Semaphore> mImageRenderFinishedSemaphores;
    TArray<vk::Fence>     mInFlightFences;

    TArray<IGraphicsPipeline*> mRenderGraphicsPipelines;
};

RHI_VULKAN_NAMESPACE_END
