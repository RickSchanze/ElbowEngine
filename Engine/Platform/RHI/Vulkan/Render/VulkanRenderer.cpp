/**
 * @file VulkanRenderer.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "VulkanRenderer.h"

#include "CoreGlobal.h"
#include "RenderPass.h"
#include "RHI/Vulkan/Instance.h"

RHI_VULKAN_NAMESPACE_BEGIN

VulkanRenderer::~VulkanRenderer() {
    if (IsValid()) {
        Finitialize();
    }
}

SharedPtr<VulkanRenderer> VulkanRenderer::CreateShared(const Instance& InVulkanInstance, const SharedPtr<IRenderPassProducer>& Producer) {
    return MakeShared<VulkanRenderer>(Protected{}, InVulkanInstance, Producer);
}

VulkanRenderer::VulkanRenderer(Protected, const Instance& InVulkanInstance, const SharedPtr<IRenderPassProducer>& Producer) {
    mRendererID = sRendererIDCount++;
    LOG_INFO_CATEGORY(Vulkan, L"创建Vulkan渲染器[id = {}]中", mRendererID, mSwapChainImageCount);
    mLogicalDevice = InVulkanInstance.CreateLogicalDeviceUnique();
    mSwapChain     = mLogicalDevice->CreateSwapChain(mSwapChainImageCount, 1920, 1080);

    if (Producer) {
        mRenderPassProducer = Producer;
    } else {
        // 寻找支持的深度图像格式
        const auto DepthFormat = mLogicalDevice->GetAssociatedPhysicalDevice()->FindSupportFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
        );
        const auto SwapchainImageFormat      = mSwapChain->GetImageFormat();
        const auto DefaultRenderPassProducer = MakeShared<Vulkan::DefaultRenderPassProducer>(SwapchainImageFormat, DepthFormat);
        mRenderPassProducer                  = DefaultRenderPassProducer;
    }
    Initialize();
}

void VulkanRenderer::Initialize() {
    const auto RenderPassInfo = mRenderPassProducer->GetRenderPassCreateInfo();
    mRenderPass               = RenderPass::CreateShared(RenderPassInfo, mLogicalDevice.get());
    LOG_INFO_CATEGORY(Vulkan, L"Vulkan渲染器[id = {}]创建完成", mRendererID, mSwapChainImageCount);
}

void VulkanRenderer::Finitialize() {
    if (!IsValid()) return;
    mRenderPass->Finialize();
    mSwapChain->Finialize();
    mLogicalDevice->Finialize();
    LOG_INFO_CATEGORY(Vulkan, L"Vukan渲染器[id = {}]清理完成", mRendererID);
}

void VulkanRenderer::Draw() {}

bool VulkanRenderer::IsValid() const {
    return mLogicalDevice->IsValid() && mSwapChain->IsValid();
}

RHI_VULKAN_NAMESPACE_END
