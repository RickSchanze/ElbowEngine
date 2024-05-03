/**
 * @file VulkanRenderer.h
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#pragma once
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "SwapChain.h"

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

class VulkanRenderer : public std::enable_shared_from_this<VulkanRenderer> {
public:
    VulkanRenderer() = default;

    virtual ~VulkanRenderer();

    static UniquePtr<VulkanRenderer> CreateUnique(const Instance& InVulkanInstance, UniquePtr<IRenderPassProducer> Producer = nullptr);

protected:
    struct Protected
    {};

public:
    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanRenderer(Protected, const Instance& InVulkanInstance);

public:
    void Initialize();
    void Finalize();

    void Draw();

    bool IsValid() const;

    UniquePtr<LogicalDevice>&       GetLogicalDevice() { return mLogicalDevice; }
    const UniquePtr<LogicalDevice>& GetLogicalDevice() const { return mLogicalDevice; }

    vk::Format                   GetSwapChainImageFormat() const { return mSwapChain->GetImageFormat(); }
    vk::Extent2D                 GetSwapChainExtent() const { return mSwapChain->GetExtent(); }
    uint32                       GetSwapChainImageCount() const { return mSwapChainImageCount; }
    Array<SharedPtr<ImageView>>& GetSwapChainImageViews() const { return mSwapChain->GetImageViews(); }

    vk::Format GetDepthFormat() const { return mDepthFormat; }

    const UniquePtr<CommandProducer>& GetCommandProducer() const { return mCommandProducer; }

    virtual void CreateGraphicsPipeline(UniquePtr<IRenderPassProducer> Producer, Ref<VulkanRenderer>InRenderer);

private:
    UniquePtr<SwapChain> mSwapChain;
    int32                mSwapChainImageCount = 3;

    SharedPtr<GraphicsPipeline> mGraphicsPipeline;

    UniquePtr<CommandProducer> mCommandProducer;

    vk::Format mDepthFormat = {};

    int32 mRendererID = 0;

    static inline int32 sRendererIDCount = 0;

    UniquePtr<LogicalDevice> mLogicalDevice;
};

RHI_VULKAN_NAMESPACE_END
