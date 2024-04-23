/**
 * @file VulkanRenderer.cpp
 * @author Echo 
 * @Date 24-4-23
 * @brief 
 */

#include "VulkanRenderer.h"

#include "CoreGlobal.h"
#include "RHI/Vulkan/Instance.h"

RHI_VULKAN_NAMESPACE_BEGIN

VulkanRenderer::~VulkanRenderer() {
    if (IsValid()) {
        Finitialize();
    }
}

SharedPtr<VulkanRenderer> VulkanRenderer::CreateShared(const Instance& InVulkanInstance) {
    return MakeShared<VulkanRenderer>(InVulkanInstance);
}

VulkanRenderer::VulkanRenderer(const Instance& InVulkanInstance) {
    mRendererID    = sRendererIDCount++;
    LOG_INFO_CATEGORY(Vulkan, L"创建Vulkan渲染器[id = {}]中\n", mRendererID, mSwapChainImageCount);
    mLogicalDevice = InVulkanInstance.CreateLogicalDevice();
    mSwapChain     = mLogicalDevice->CreateSwapChain(mSwapChainImageCount);
    Initialize();
}

void VulkanRenderer::Initialize() {
    LOG_INFO_CATEGORY(Vulkan, L"Vulkan渲染器[id = {}]创建完成", mRendererID, mSwapChainImageCount);
}

void VulkanRenderer::Finitialize() {
    if (!IsValid()) return;
    mSwapChain->Finalize();
    mLogicalDevice->Finalize();
    LOG_INFO_CATEGORY(Vulkan, L"Vukan渲染器[id = {}]清理完成", mRendererID);
}

void VulkanRenderer::Draw() {}

bool VulkanRenderer::IsValid() const {
    return mLogicalDevice->IsValid() && mSwapChain->IsValid();
}

RHI_VULKAN_NAMESPACE_END
