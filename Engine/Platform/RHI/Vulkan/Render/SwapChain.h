/**
 * @file SwapChain.h
 * @author Echo 
 * @Date 24-4-22
 * @brief 
 */

#pragma once
#include "Image.h"
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan {
class LogicalDevice;
}
RHI_VULKAN_NAMESPACE_BEGIN

class SwapChain final : public IRHIResource {
public:
    SwapChain() = default;

    static UniquePtr<SwapChain> CreateUnique(
        vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, vk::Format InSwapchainFormat,
        vk::Extent2D InSwapchainExtent
    );
    static SharedPtr<SwapChain> CreateShared(
        vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, vk::Format InSwapchainFormat,
        vk::Extent2D InSwapchainExtent
    );

    explicit SwapChain(
        ResourcePrivate, vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice, vk::Format InSwapchainFormat,
        vk::Extent2D InSwapchainExtent
    );

    ~SwapChain() override;

    // 表面格式
    static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const Array<vk::SurfaceFormatKHR>& InAvailableFormats);
    // 显示模式（垂直同步？）
    static vk::PresentModeKHR   ChooseSwapPresentMode(const Array<vk::PresentModeKHR>& InAvailablePresentModes);
    // 分辨率
    static vk::Extent2D         ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& InCapabilities, uint32 InWidth, uint32 InHeight);

    [[nodiscard]] vk::SwapchainKHR GetHandle() const { return mSwapchainHandle; }
    [[nodiscard]] LogicalDevice&   GetAssociatedLogicalDevice() const { return *mAssociatedLogicalDevice; }

    [[nodiscard]] bool IsValid() const override;

    vk::Format   GetImageFormat() const {
        return mSwapchainImageFormat;
    }
    vk::Extent2D GetExtent() const { return mSwapchainExtent; }

    void Initialize() override;
    void Finialize() override;

    uint32 GetSwapchainImageCount() const { return static_cast<uint32>(mSwapchainImages.size()); }

private:
    vk::SwapchainKHR                 mSwapchainHandle;
    Array<SharedPtr<SwapChainImage>> mSwapchainImages;
    Array<SharedPtr<ImageView>>      mSwapchainImageViews;
    vk::Format                       mSwapchainImageFormat;
    vk::Extent2D                     mSwapchainExtent;

    LogicalDevice* mAssociatedLogicalDevice = nullptr;
};

RHI_VULKAN_NAMESPACE_END
