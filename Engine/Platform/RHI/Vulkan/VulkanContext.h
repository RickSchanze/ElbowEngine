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
class CommandProducer;
class GraphicsPipeline;
}   // namespace RHI::Vulkan
namespace RHI::Vulkan {
class IRenderPassProducer;
}
namespace RHI::Vulkan {
class RenderPass;
}

RHI_VULKAN_NAMESPACE_BEGIN

class VulkanContext {
public:
    VulkanContext() = default;

    virtual ~VulkanContext();

    static TUniquePtr<VulkanContext>
    CreateUnique(const TSharedPtr<Instance>& InVulkanInstance, TUniquePtr<IRenderPassProducer> Producer = nullptr);

protected:
    struct Protected
    {};

public:
    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanContext(Protected, const TSharedPtr<Instance>& InVulkanInstance);

    void Initialize();
    void Finalize();

    void Draw();

    bool IsValid() const;

    // 重建交换链
    void RebuildSwapChain();

    vk::Format                   GetSwapChainImageFormat() const { return mSwapChain->GetImageFormat(); }
    vk::Extent2D                 GetSwapChainExtent() const { return mSwapChain->GetExtent(); }
    uint32                       GetSwapChainImageCount() const { return mSwapChainImageCount; }
    TArray<TSharedPtr<ImageView>>& GetSwapChainImageViews() const { return mSwapChain->GetImageViews(); }

    vk::Format GetDepthFormat() const { return mDepthFormat; }

    const TUniquePtr<CommandProducer>& GetCommandProducer() const { return mCommandProducer; }

    virtual void CreateGraphicsPipeline(TUniquePtr<IRenderPassProducer> Producer, Ref<VulkanContext> InRenderer);

    // 向渲染器提交命令缓冲区
    void AddPipelineToRender(IGraphicsPipeline* InPipeline) {
        mRenderGraphicsPipelines.push_back(InPipeline);
    }
    void RemovePipelineFromRender(IGraphicsPipeline* InPipeline) {
        ContainerUtils::Remove(mRenderGraphicsPipelines, InPipeline);
    }

    TUniquePtr<SwapChain>&       GetSwapChain() { return mSwapChain; }
    TUniquePtr<CommandProducer>& GetCommandProducer() { return mCommandProducer; }
    TUniquePtr<LogicalDevice>&   GetLogicalDevice() { return mLogicalDevice; }
    TUniquePtr<PhysicalDevice>&  GetPhysicalDevice() { return mPhysicalDevice; }
    TSharedPtr<Instance>         GetVulkanInstance() { return mVulkanInstance; }

protected:
    void CreateSyncObjecs();
    void CleanSyncObjects();

private:
    int32 mSwapChainImageCount = 3;

    int32 mMaxFramesInFlight = 2;
    int   mCurrentFrame      = 0;

    vk::Format mDepthFormat = {};

    int32 mRendererID = 0;

    static inline int32 sRendererIDCount = 0;

    // TODO: 图形管线和Shader一起属于材质系统
    TSharedPtr<GraphicsPipeline> mGraphicsPipeline;

    TUniquePtr<SwapChain>       mSwapChain;
    TUniquePtr<CommandProducer> mCommandProducer;
    TUniquePtr<LogicalDevice>   mLogicalDevice;
    TUniquePtr<PhysicalDevice>  mPhysicalDevice;
    TSharedPtr<Instance>        mVulkanInstance;

    TArray<vk::Semaphore> mImageAvailableSemaphores;
    TArray<vk::Semaphore> mImageRenderFinishedSemaphores;
    TArray<vk::Fence>     mInFlightFences;

    TArray<IGraphicsPipeline*> mRenderGraphicsPipelines;
};

RHI_VULKAN_NAMESPACE_END
