/**
 * @file ImageView.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan
{
class LogicalDevice;
}
RHI_VULKAN_NAMESPACE_BEGIN

class ImageView : public IRHIResource
{
public:
    explicit ImageView() = default;

    explicit ImageView(const vk::ImageView& InViewHandle) : mViewHandle(InViewHandle) {}

    ~ImageView() override;

    bool IsValid() const { return static_cast<bool>(mViewHandle); }

    vk::ImageView GetHandle() const { return mViewHandle; }

    void InternalDestroy();

    void Destroy() override;

private:
    vk::ImageView mViewHandle = VK_NULL_HANDLE;
};

RHI_VULKAN_NAMESPACE_END
