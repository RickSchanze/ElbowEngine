/**
 * @file Image_Vulkan.h
 * @author Echo 
 * @Date 24-11-25
 * @brief 
 */

#pragma once
#include "Platform/RHI/Image.h"
#include "Platform/RHI/ImageView.h"
#include "vulkan/vulkan.hpp"

namespace platform::rhi::vulkan
{
class GfxContext_Vulkan;

class Image_Vulkan final : public Image
{
    friend class GfxContext_Vulkan;

public:
    explicit Image_Vulkan(const ImageDesc& desc);

    /// **仅** 供交换链图像使用
    explicit Image_Vulkan(VkImage handle_, int32_t index, uint32_t width_, uint32_t height_, Format format_);

    ~Image_Vulkan() override;

public:
    [[nodiscard]] void* GetNativeHandle() const override { return image_handle_; }

    [[nodiscard]] VkDeviceMemory GetNativeMemory() const { return image_memory_; }

    void UploadData(const void* data, const size_t size) override;

protected:
    VkImage        image_handle_ = VK_NULL_HANDLE;
    VkDeviceMemory image_memory_ = VK_NULL_HANDLE;
};
}   // namespace platform::rhi::vulkan
