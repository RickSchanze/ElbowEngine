/**
 * @file VulkanRenderer.h
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#pragma once
#include "LogicalDevice.h"
#include "RenderPass.h"
#include "RHI/Vulkan/Instance.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "SwapChain.h"

namespace RHI::Vulkan {
class GraphicsPipeline;
}
namespace RHI::Vulkan {
class IRenderPassProducer;
}
namespace RHI::Vulkan {
class RenderPass;
}

RHI_VULKAN_NAMESPACE_BEGIN

class VulkanRenderer {
public:
    VulkanRenderer() = default;

    ~VulkanRenderer();

    static SharedPtr<VulkanRenderer> CreateShared(const Instance& InVulkanInstance, UniquePtr<IRenderPassProducer> Producer = nullptr);

protected:
    struct Protected
    {};

public:
    // 请不要直接调用此函数，请使用VulkanRenderer::Create
    explicit VulkanRenderer(Protected, const Instance& InVulkanInstance, UniquePtr<IRenderPassProducer> Producer = nullptr);

public:
    void Initialize();
    void Finitialize();

    void Draw();

    bool IsValid() const;

    SharedPtr<LogicalDevice> GetLogicalDevice() const { return mLogicalDevice; }
    vk::Format               GetSwapChainImageFormat() const { return mSwapChain->GetImageFormat(); }
    vk::Extent2D             GetSwapChainExtent() const { return mSwapChain->GetExtent(); }

private:
    UniquePtr<SwapChain> mSwapChain;
    int32                mSwapChainImageCount = 3;

    SharedPtr<GraphicsPipeline> mGraphicsPipeline;

    int32 mRendererID = 0;

    static inline int32 sRendererIDCount = 0;

    SharedPtr<LogicalDevice> mLogicalDevice;
};

RHI_VULKAN_NAMESPACE_END
