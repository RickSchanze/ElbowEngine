/**
 * @file ImageView_Vulkan.h
 * @author Echo 
 * @Date 24-11-25
 * @brief 
 */

#pragma once
#include "Platform/RHI/ImageView.h"
#include "vulkan/vulkan.h"

namespace platform::rhi::vulkan {

VkComponentMapping FromComponentMapping(const ComponentMapping& mapping);

class ImageView_Vulkan : public ImageView
{
    friend class platform::rhi::Image;
public:
    explicit ImageView_Vulkan(const ImageViewDesc& desc);

    [[nodiscard]] void* GetNativeHandle() const override { return reinterpret_cast<void*>(handle_); }

private:
    VkImageView handle_ = VK_NULL_HANDLE;
};

}
