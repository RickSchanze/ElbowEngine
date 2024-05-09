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
#include "VulkanCommon.h"

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

    static UniquePtr<VulkanContext>
    CreateUnique(const SharedPtr<Instance>& InVulkanInstance, UniquePtr<IRenderPassProducer> Producer = nullptr);

protected:
    struct Protected
    {};

public:
    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanContext(Protected, const SharedPtr<Instance>& InVulkanInstance);

    void Initialize();
    void Finalize();

    void Draw();

    bool IsValid() const;

    // 重建交换链
    void RebuildSwapChain();

    vk::Format                   GetSwapChainImageFormat() const { return mSwapChain->GetImageFormat(); }
    vk::Extent2D                 GetSwapChainExtent() const { return mSwapChain->GetExtent(); }
    uint32                       GetSwapChainImageCount() const { return mSwapChainImageCount; }
    Array<SharedPtr<ImageView>>& GetSwapChainImageViews() const { return mSwapChain->GetImageViews(); }

    vk::Format GetDepthFormat() const { return mDepthFormat; }

    const UniquePtr<CommandProducer>& GetCommandProducer() const { return mCommandProducer; }

    virtual void CreateGraphicsPipeline(UniquePtr<IRenderPassProducer> Producer, Ref<VulkanContext> InRenderer);

    UniquePtr<SwapChain>&       GetSwapChain() { return mSwapChain; }
    UniquePtr<CommandProducer>& GetCommandProducer() { return mCommandProducer; }
    UniquePtr<LogicalDevice>&   GetLogicalDevice() { return mLogicalDevice; }
    UniquePtr<PhysicalDevice>&  GetPhysicalDevice() { return mPhysicalDevice; }
    SharedPtr<Instance>         GetVulkanInstance() { return mVulkanInstance; }

protected:
    // 初始化VulkanInstance
    void CreateInstance();

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
    SharedPtr<GraphicsPipeline> mGraphicsPipeline;

    UniquePtr<SwapChain>       mSwapChain;
    UniquePtr<CommandProducer> mCommandProducer;
    UniquePtr<LogicalDevice>   mLogicalDevice;
    UniquePtr<PhysicalDevice>  mPhysicalDevice;
    SharedPtr<Instance>        mVulkanInstance;

    Array<vk::Semaphore> mImageAvailableSemaphores;
    Array<vk::Semaphore> mImageRenderFinishedSemaphores;
    Array<vk::Fence>     mInFlightFences;
};

RHI_VULKAN_NAMESPACE_END
