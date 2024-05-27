/**
 * @file SwapChain.cpp
 * @author Echo 
 * @Date 24-4-22
 * @brief 
 */

#include "SwapChain.h"

#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RHI/Vulkan/VulkanStringify.h"

RHI_VULKAN_NAMESPACE_BEGIN

TUniquePtr<SwapChain> SwapChain::CreateUnique(
    vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, vk::Format InSwapchainFormat,
    vk::Extent2D InSwapchainExtent
) {
    return MakeUnique<SwapChain>(ResourceProtected{}, InSwapchainHandle, InAssociatedLogicalDevice, InSwapchainFormat, InSwapchainExtent);
}

TSharedPtr<SwapChain> SwapChain::CreateShared(
    vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, vk::Format InSwapchainFormat,
    vk::Extent2D InSwapchainExtent
) {
    return MakeShared<SwapChain>(ResourceProtected{}, InSwapchainHandle, InAssociatedLogicalDevice, InSwapchainFormat, InSwapchainExtent);
}

SwapChain::SwapChain(
    ResourceProtected, const vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, const vk::Format InSwapchainFormat,
    const vk::Extent2D InSwapchainExtent
) :
    mSwapchainHandle(InSwapchainHandle), mSwapchainImageFormat(InSwapchainFormat), mSwapchainExtent(InSwapchainExtent),
    mAssociatedLogicalDevice(InAssociatedLogicalDevice) {
    Initialize();
}

SwapChain::~SwapChain() {
    if (IsValid()) {
        Finialize();
    }
}

vk::SurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const TArray<vk::SurfaceFormatKHR>& InAvailableFormats) {
    // 看看设定的格式在不在列表
    for (const auto& AvailableFormat: InAvailableFormats) {
        if (AvailableFormat.format == vk::Format::eB8G8R8Unorm && AvailableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return AvailableFormat;
        }
    }
    // 找不到直接返回第一个
    return InAvailableFormats[0];
}

vk::PresentModeKHR SwapChain::ChooseSwapPresentMode(const TArray<vk::PresentModeKHR>& InAvailablePresentModes) {
    // FIFO: 垂直同步
    auto BestMode = vk::PresentModeKHR::eFifo;
    for (const auto& AvailablePresentMode: InAvailablePresentModes) {
        // Mailbox 混合垂直同步和非垂直同步
        if (AvailablePresentMode == vk::PresentModeKHR::eMailbox) {
            return AvailablePresentMode;
        }
        // Immediate 不等待垂直同步
        if (AvailablePresentMode == vk::PresentModeKHR::eImmediate) {
            BestMode = AvailablePresentMode;
        }
    }
    return BestMode;
}

vk::Extent2D SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& InCapabilities, const uint32 InWidth, const uint32 InHeight) {
    // 分辨率不是无限的
    if (InCapabilities.currentExtent.width != UINT32_MAX) {
        return InCapabilities.currentExtent;
    }
    VkExtent2D ActualExtent = {InWidth, InHeight};
    ActualExtent.width      = std::clamp(ActualExtent.width, InCapabilities.minImageExtent.width, InCapabilities.maxImageExtent.width);
    ActualExtent.height     = std::clamp(ActualExtent.height, InCapabilities.minImageExtent.height, InCapabilities.maxImageExtent.height);
    return ActualExtent;
}

bool SwapChain::IsValid() const {
    return mSwapchainHandle && mAssociatedLogicalDevice && mAssociatedLogicalDevice->IsValid() && !mSwapchainImages.empty();
}

void SwapChain::Initialize() {
    if (IsValid()) return;
    auto SwapchainImages = mAssociatedLogicalDevice->GetHandle().getSwapchainImagesKHR(mSwapchainHandle);
    mSwapchainImages.resize(SwapchainImages.size());
    std::ranges::transform(SwapchainImages, mSwapchainImages.begin(), [](const auto& ImageHandle) { return MakeShared<SwapChainImage>(ImageHandle); });

    mSwapchainImageViews.resize(GetSwapchainImageCount());
    for (uint32 i = 0; i < GetSwapchainImageCount(); ++i) {
        mSwapchainImageViews[i] =
            mAssociatedLogicalDevice->CreateImageView(*mSwapchainImages[i], mSwapchainImageFormat, vk::ImageAspectFlagBits::eColor, 1);
    }

    LOG_INFO_CATEGORY(
        VULKAN, L"交换链创建完成, 交换链图像数: {}, 范围: {}", mSwapchainImages.size(), VulkanStringify::ToString(mSwapchainExtent)
    );
}

void SwapChain::Finialize() {
    if (!IsValid()) return;
    for (const auto& ImageView: mSwapchainImageViews) {
        ImageView->Finialize();
    }
    mAssociatedLogicalDevice->GetHandle().destroy(mSwapchainHandle);
    mSwapchainHandle = VK_NULL_HANDLE;
    mSwapchainImages.clear();
    LOG_INFO_CATEGORY(VULKAN, L"交换链清理完成");
}

void SwapChain::Destroy(){
    Finialize();
}

RHI_VULKAN_NAMESPACE_END
