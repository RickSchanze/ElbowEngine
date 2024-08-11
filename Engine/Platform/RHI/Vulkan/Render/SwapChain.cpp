/**
 * @file SwapChain.cpp
 * @author Echo 
 * @Date 24-4-22
 * @brief 
 */

#include "SwapChain.h"

#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanStringify.h"

RHI_VULKAN_NAMESPACE_BEGIN

TUniquePtr<SwapChain> SwapChain::CreateUnique(
    vk::SwapchainKHR swapchain_handle, LogicalDevice* associated_logical_device, vk::Format swapchain_format, vk::Extent2D swapchain_extent
)
{
    return MakeUnique<SwapChain>(ResourceProtected{}, swapchain_handle, associated_logical_device, swapchain_format, swapchain_extent);
}

TSharedPtr<SwapChain> SwapChain::CreateShared(
    vk::SwapchainKHR swapchain_handle, LogicalDevice* associated_logical_device, vk::Format swapchain_format, vk::Extent2D swapchain_extent
)
{
    return MakeShared<SwapChain>(ResourceProtected{}, swapchain_handle, associated_logical_device, swapchain_format, swapchain_extent);
}

SwapChain::SwapChain(
    ResourceProtected, const vk::SwapchainKHR swapchain_handle, LogicalDevice* associated_logical_device, const vk::Format swapchain_format,
    const vk::Extent2D swapchain_extent
) :
    swapchain_handle_(swapchain_handle), swapchain_image_format_(swapchain_format), swapchain_extent_(swapchain_extent),
    associated_logical_device_(associated_logical_device)
{
    Initialize();
}

SwapChain::~SwapChain()
{
    if (IsValid())
    {
        Finialize();
    }
}

vk::SurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const TArray<vk::SurfaceFormatKHR>& available_formats)
{
    // 看看设定的格式在不在列表
    for (const auto& AvailableFormat: available_formats)
    {
        if (AvailableFormat.format == vk::Format::eB8G8R8Unorm && AvailableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return AvailableFormat;
        }
    }
    // 找不到直接返回第一个
    return available_formats[0];
}

vk::PresentModeKHR SwapChain::ChooseSwapPresentMode(const TArray<vk::PresentModeKHR>& available_present_modes)
{
    // FIFO: 垂直同步
    auto BestMode = vk::PresentModeKHR::eFifo;
    for (const auto& AvailablePresentMode: available_present_modes)
    {
        // Mailbox 混合垂直同步和非垂直同步
        if (AvailablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return AvailablePresentMode;
        }
        // Immediate 不等待垂直同步
        if (AvailablePresentMode == vk::PresentModeKHR::eImmediate)
        {
            BestMode = AvailablePresentMode;
        }
    }
    return BestMode;
}

vk::Extent2D SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const uint32_t width, const uint32_t height)
{
    // 分辨率不是无限的
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    VkExtent2D ActualExtent = {width, height};
    ActualExtent.width      = std::clamp(ActualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    ActualExtent.height     = std::clamp(ActualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return ActualExtent;
}

bool SwapChain::IsValid() const
{
    return swapchain_handle_ && associated_logical_device_ && associated_logical_device_->IsValid() && !swap_chain_images_.empty();
}

void SwapChain::Initialize()
{
    // clang-format off
    static const char* s_swap_chain_image_view_debug_name[3] = {
        "SwapchainImageView_0",
        "SwapchainImageView_1",
        "SwapchainImageView_2"
    };

    static const char* s_swap_chain_image_debug_name[3] = {
        "SwapchainImage_0",
        "SwapchainImage_1",
        "SwapchainImage_2",
    };
    // clang-format on

    if (IsValid()) return;
    auto swapchain_images = associated_logical_device_->GetHandle().getSwapchainImagesKHR(swapchain_handle_);
    swap_chain_images_.resize(swapchain_images.size());
    for (int i = 0; i < swapchain_images.size(); i++)
    {
        swap_chain_images_[i] = MakeShared<SwapChainImage>(swapchain_images[i]);
        VulkanContext::Get()->GetLogicalDevice()->SetImageDebugName(swapchain_images[i], s_swap_chain_image_debug_name[i]);
    }

    swapchain_image_views_.resize(GetSwapchainImageCount());
    for (uint32_t i = 0; i < GetSwapchainImageCount(); ++i)
    {
        ImageViewInfo view_info;
        view_info.format = swapchain_image_format_;
        view_info.name = s_swap_chain_image_view_debug_name[i];
        swapchain_image_views_[i] = swap_chain_images_[i]->CreateImageViewShared(view_info);
    }

    LOG_INFO_CATEGORY(VULKAN, L"交换链创建完成, 交换链图像数: {}, 范围: {}", swap_chain_images_.size(), VulkanStringify::ToString(swapchain_extent_));
}

void SwapChain::Finialize()
{
    if (!IsValid()) return;
    for (const auto& ImageView: swapchain_image_views_)
    {
        ImageView->InternalDestroy();
    }
    associated_logical_device_->DestroySwapChain(swapchain_handle_);
    swapchain_handle_ = VK_NULL_HANDLE;
    swap_chain_images_.clear();
    LOG_INFO_CATEGORY(VULKAN, L"交换链清理完成");
}

void SwapChain::Destroy(){
    Finialize();
}

RHI_VULKAN_NAMESPACE_END
