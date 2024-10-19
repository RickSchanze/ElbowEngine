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
namespace rhi::vulkan
{
class LogicalDevice;

class SwapChain final : public IRHIResource
{
public:
    static UniquePtr<SwapChain> CreateUnique(
        vk::SwapchainKHR swapchain_handle, LogicalDevice* associated_logical_device, vk::Format swapchain_format, vk::Extent2D swapchain_extent
    );
    static SharedPtr<SwapChain> CreateShared(
        vk::SwapchainKHR swapchain_handle, LogicalDevice* associated_logical_device, vk::Format swapchain_format, vk::Extent2D swapchain_extent
    );

    explicit SwapChain(
        ResourceProtected, vk::SwapchainKHR swapchain_handle, LogicalDevice* associated_logical_device, vk::Format swapchain_format,
        vk::Extent2D swapchain_extent
    );

    ~SwapChain() override;

    // 表面格式
    static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const Array<vk::SurfaceFormatKHR>& available_formats);
    // 显示模式（垂直同步？）
    static vk::PresentModeKHR   ChooseSwapPresentMode(const Array<vk::PresentModeKHR>& available_present_modes);
    // 分辨率
    static vk::Extent2D         ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    vk::SwapchainKHR GetHandle() const { return swapchain_handle_; }

    LogicalDevice& GetAssociatedLogicalDevice() const { return *associated_logical_device_; }

    bool IsValid() const;

    vk::Format GetImageFormat() const { return swapchain_image_format_; }

    vk::Extent2D GetExtent() const { return swapchain_extent_; }

    Array<SharedPtr<ImageView>>& GetImageViews() { return swapchain_image_views_; }

    void Initialize();
    void DeInitialize(bool log);

    int32_t GetSwapchainImageCount() const { return static_cast<uint32_t>(swap_chain_images_.size()); }

    void Destroy() override;

private:
    vk::SwapchainKHR                   swapchain_handle_;
    Array<SharedPtr<SwapChainImage>> swap_chain_images_;
    Array<SharedPtr<ImageView>>      swapchain_image_views_;
    vk::Format                         swapchain_image_format_;
    vk::Extent2D                       swapchain_extent_;

    LogicalDevice* associated_logical_device_ = nullptr;
};

}
