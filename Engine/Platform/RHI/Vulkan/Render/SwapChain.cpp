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

UniquePtr<SwapChain> SwapChain::CreateUnique(vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice) {
    return MakeUnique<SwapChain>(InSwapchainHandle, InAssociatedLogicalDevice, ResourcePrivate{});
}

SharedPtr<SwapChain> SwapChain::CreateShared(vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice) {
    return MakeShared<SwapChain>(InSwapchainHandle, InAssociatedLogicalDevice, ResourcePrivate{});
}

SwapChain::SwapChain(const vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, ResourcePrivate) :
    mSwapchainHandle(InSwapchainHandle), mAssociatedLogicalDevice(InAssociatedLogicalDevice) {
    Initialize();
}

SwapChain::~SwapChain() {
    if (IsValid()) {
        Finalize();
    }
}

vk::SurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const Array<vk::SurfaceFormatKHR>& availableFormats) {
    return availableFormats[0];
}

vk::PresentModeKHR SwapChain::ChooseSwapPresentMode(const Array<vk::PresentModeKHR>& availablePresentModes) {
    return availablePresentModes[0];
}

vk::Extent2D SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    return capabilities.currentExtent;
}

bool SwapChain::IsValid() const {
    return mSwapchainHandle && mAssociatedLogicalDevice && mAssociatedLogicalDevice->IsValid() && !mSwapchainImages.empty();
}

void SwapChain::Initialize() {
    if (IsValid()) return;
    auto SwapchainImages = mAssociatedLogicalDevice->GetLogicalDeviceHandle().getSwapchainImagesKHR(mSwapchainHandle);
    mSwapchainImages.resize(SwapchainImages.size());
    std::ranges::transform(SwapchainImages, mSwapchainImages.begin(), [](const auto& ImageHandle) { return Image(ImageHandle); });
    LOG_INFO_CATEGORY(VULKAN, L"交换链创建完成, 交换链图像数: {}", mSwapchainImages.size());
}

void SwapChain::Finalize() {
    if (!IsValid()) return;
    mAssociatedLogicalDevice->GetLogicalDeviceHandle().destroy(mSwapchainHandle);
    mSwapchainHandle = VK_NULL_HANDLE;
    mSwapchainImages.clear();
    LOG_INFO_CATEGORY(VULKAN, L"交换链清理完成");
}

RHI_VULKAN_NAMESPACE_END
