/**
 * @file SwapChain.h
 * @author Echo 
 * @Date 24-4-22
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/Resource/Image.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan
{
class LogicalDevice;
}
RHI_VULKAN_NAMESPACE_BEGIN

class SwapChain final : public IRHIResource
{
public:
    SwapChain();

    static TUniquePtr<SwapChain> CreateUnique(
        vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, vk::Format InSwapchainFormat, vk::Extent2D InSwapchainExtent
    );
    static TSharedPtr<SwapChain> CreateShared(
        vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, vk::Format InSwapchainFormat, vk::Extent2D InSwapchainExtent
    );

    explicit SwapChain(
        ResourceProtected, vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, vk::Format InSwapchainFormat,
        vk::Extent2D InSwapchainExtent
    );

    ~SwapChain() override;

    // 表面格式
    static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const TArray<vk::SurfaceFormatKHR>& InAvailableFormats);
    // 显示模式（垂直同步？）
    static vk::PresentModeKHR   ChooseSwapPresentMode(const TArray<vk::PresentModeKHR>& InAvailablePresentModes);
    // 分辨率
    static vk::Extent2D         ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& InCapabilities, uint32_t InWidth, uint32_t InHeight);

    [[nodiscard]] vk::SwapchainKHR GetHandle() const { return mSwapchainHandle; }
    [[nodiscard]] LogicalDevice&   GetAssociatedLogicalDevice() const { return *mAssociatedLogicalDevice; }

    [[nodiscard]] bool IsValid() const;

    vk::Format                     GetImageFormat() const { return mSwapchainImageFormat; }
    vk::Extent2D                   GetExtent() const { return mSwapchainExtent; }
    TArray<TSharedPtr<ImageView>>& GetImageViews() { return mSwapchainImageViews; }

    void Initialize();
    void Finialize();

    int32_t GetSwapchainImageCount() const { return static_cast<uint32_t>(mSwapchainImages.size()); }

    void Destroy() override;

private:
    vk::SwapchainKHR                   mSwapchainHandle;
    TArray<TSharedPtr<SwapChainImage>> mSwapchainImages;
    TArray<TSharedPtr<ImageView>>      mSwapchainImageViews;
    vk::Format                         mSwapchainImageFormat;
    vk::Extent2D                       mSwapchainExtent;

    LogicalDevice* mAssociatedLogicalDevice = nullptr;
};

RHI_VULKAN_NAMESPACE_END
