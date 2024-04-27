/**
 * @file ImageView.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan {
class LogicalDevice;
}
RHI_VULKAN_NAMESPACE_BEGIN

class ImageView {
public:
    explicit ImageView() = default;

    explicit ImageView(const vk::ImageView& InViewHandle, LogicalDevice* InLogicalDevice) :
        mViewHandle(InViewHandle), mAssociatedLogicalDevice(InLogicalDevice) {}

    ~ImageView();

    bool          IsValid() const { return static_cast<bool>(mViewHandle); }
    vk::ImageView GetHandle() const { return mViewHandle; }

    void Initialize() {}
    void Finialize();

private:
    vk::ImageView  mViewHandle = VK_NULL_HANDLE;
    LogicalDevice* mAssociatedLogicalDevice = nullptr;
};

RHI_VULKAN_NAMESPACE_END
