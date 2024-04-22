/**
 * @file SwapChain.h
 * @author Echo 
 * @Date 24-4-22
 * @brief 
 */

#pragma once
#include "Interface/IRHIResource.h"
#include "vulkan/vulkan.hpp"
#include "VulkanCommon.h"

namespace RHI::Vulkan {
class LogicalDevice;
}
RHI_VULKAN_NAMESPACE_BEGIN

class SwapChain : public IRHIResource {
public:
    SwapChain() = default;
    explicit SwapChain(const vk::SwapchainKHR InSwapchainHandle, LogicalDevice* InAssociatedLogicalDevice) :
        mSwapchainHandle(InSwapchainHandle), mAssociatedLogicalDevice(InAssociatedLogicalDevice) {}

    static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const Array<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR   ChooseSwapPresentMode(const Array<vk::PresentModeKHR>& availablePresentModes);
    static vk::Extent2D         ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    [[nodiscard]] vk::SwapchainKHR GetSwapchainHandle() const { return mSwapchainHandle; }
    [[nodiscard]] LogicalDevice&   GetAssociatedLogicalDevice() const { return *mAssociatedLogicalDevice; }

    [[nodiscard]] bool IsValid() const override { return static_cast<bool>(mSwapchainHandle) && mAssociatedLogicalDevice != nullptr; }
    void               Initialize() override;
    void               Finalize() override;

private:
    vk::SwapchainKHR mSwapchainHandle;
    LogicalDevice*   mAssociatedLogicalDevice = nullptr;
};

RHI_VULKAN_NAMESPACE_END
