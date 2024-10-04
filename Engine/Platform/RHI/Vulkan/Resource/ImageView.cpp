/**
 * @file ImageView.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#include "ImageView.h"

#include "RHI/Vulkan/Render/LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"

namespace rhi::vulkan
{
ImageView::ImageView(const vk::ImageView& handle, const char* debug_name)
{
    handle_ = handle;
    if (debug_name)
    {
        debug_name_ = debug_name;
        VulkanContext::Get()->GetLogicalDevice()->SetImageViewDebugName(handle, debug_name_.data());
    }
}

ImageView::~ImageView()
{
    InternalDestroy();
}

void ImageView::InternalDestroy()
{
    if (!IsValid()) return;
    VulkanContext::Get()->GetLogicalDevice()->GetHandle().destroy(handle_);
    handle_ = VK_NULL_HANDLE;
}

void ImageView::Destroy()
{
    InternalDestroy();
}
}   // namespace rhi::vulkan
