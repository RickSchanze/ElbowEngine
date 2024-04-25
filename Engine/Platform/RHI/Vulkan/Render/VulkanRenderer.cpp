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
    mLogicalDevice = InVulkanInstance.CreateLogicalDevice();

    if (Producer) {
        mRenderPassProducer = Producer;
    } else {
        const auto DefaultRenderPassProducer = MakeShared<Vulkan::DefaultRenderPassProducer>();
        mRenderPassProducer                  = DefaultRenderPassProducer;
    }
    Initialize();
}

void VulkanRenderer::Initialize() {
    mSwapChain = mLogicalDevice->CreateSwapChain(mSwapChainImageCount);
    const auto RenderPassInfo = mRenderPassProducer->GetRenderPassCreateInfo();
    mRenderPass               = RenderPass::CreateShared(RenderPassInfo, mLogicalDevice);
    LOG_INFO_CATEGORY(Vulkan, L"Vulkan渲染器[id = {}]创建完成", mRendererID, mSwapChainImageCount);
}

void VulkanRenderer::Finitialize() {
    if (!IsValid()) return;
    mRenderPass->Finalize();
    mSwapChain->Finalize();
    mLogicalDevice->Finalize();
    LOG_INFO_CATEGORY(Vulkan, L"Vukan渲染器[id = {}]清理完成", mRendererID);
}

void VulkanRenderer::Draw() {}

bool VulkanRenderer::IsValid() const {
    return mLogicalDevice->IsValid() && mSwapChain->IsValid();
}

RHI_VULKAN_NAMESPACE_END
