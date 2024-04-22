/**
 * @file SwapChain.cpp
 * @author Echo 
 * @Date 24-4-22
 * @brief 
 */

#include "SwapChain.h"

#include "CoreGlobal.h"
#include "LogicalDevice.h"

RHI_VULKAN_NAMESPACE_BEGIN

vk::SurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const Array<vk::SurfaceFormatKHR>& availableFormats) {
    return availableFormats[0];
}

vk::PresentModeKHR SwapChain::ChooseSwapPresentMode(const Array<vk::PresentModeKHR>& availablePresentModes) {
    return availablePresentModes[0];
}

vk::Extent2D SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    return capabilities.currentExtent;
}

void SwapChain::Initialize() {
    LOG_INFO_CATEGORY(VULKAN, L"初始化交换链完成");
}

void SwapChain::Finalize() {
    mAssociatedLogicalDevice->GetLogicalDeviceHandle().destroy(mSwapchainHandle);
    LOG_INFO_CATEGORY(VULKAN, L"清理交换链完成");
}

RHI_VULKAN_NAMESPACE_END
