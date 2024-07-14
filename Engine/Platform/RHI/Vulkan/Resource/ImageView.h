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

    explicit ImageView(const vk::ImageView& handle, const char* debug_name = nullptr);

    ~ImageView() override;

    bool IsValid() const { return static_cast<bool>(handle_); }

    vk::ImageView GetHandle() const { return handle_; }

    void InternalDestroy();

    void Destroy() override;

private:
    vk::ImageView handle_ = VK_NULL_HANDLE;
#ifdef ELBOW_DEBUG
    AnsiString debug_name_;
#endif
};

RHI_VULKAN_NAMESPACE_END
