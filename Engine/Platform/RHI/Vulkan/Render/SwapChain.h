/**
 * @file SwapChain.h
 * @author Echo 
 * @Date 24-4-22
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "Image.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan {
class LogicalDevice;
}
RHI_VULKAN_NAMESPACE_BEGIN

class SwapChain final : public IRHIResource {
public:
    SwapChain() = default;

    static UniquePtr<SwapChain> CreateUnique(vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice);

public:
    // 请勿直接使用此函数，请使用SwapChain::Create
    explicit SwapChain(vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice);

public:
    ~SwapChain() override;

    static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const Array<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR   ChooseSwapPresentMode(const Array<vk::PresentModeKHR>& availablePresentModes);
    static vk::Extent2D         ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    [[nodiscard]] vk::SwapchainKHR GetSwapchainHandle() const { return mSwapchainHandle; }
    [[nodiscard]] LogicalDevice&   GetAssociatedLogicalDevice() const { return *mAssociatedLogicalDevice; }

    [[nodiscard]] bool IsValid() const override;

    void Initialize() override;
    void Finalize() override;

private:
    vk::SwapchainKHR mSwapchainHandle;
    Array<Image>     mSwapchainImages;

    LogicalDevice* mAssociatedLogicalDevice = nullptr;
};

RHI_VULKAN_NAMESPACE_END
