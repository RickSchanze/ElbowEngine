/**
 * @file ImageView_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-25
 * @brief 
 */

#include "ImageView_Vulkan.h"

#include "Platform/RHI/Image.h"

platform::rhi::vulkan::ImageView_Vulkan::ImageView_Vulkan(const ImageViewDesc& desc) : ImageView(desc)
{
    const auto img        = desc_.image;
    auto       img_handle = static_cast<VkImage>(img->GetNativeHandle());
    Assert(logcat::Platform_RHI_Vulkan, img, "Image cannot be nullptr when creating a ImageView.");
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = img_handle;

}
