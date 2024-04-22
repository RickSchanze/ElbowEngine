/**
 * @file SwapChain.cpp
 * @author Echo 
 * @Date 24-4-22
 * @brief 
 */

#include "SwapChain.h"

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

RHI_VULKAN_NAMESPACE_END
