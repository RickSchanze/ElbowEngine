/**
 * @file ImageView_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-25
 * @brief 
 */

#include "ImageView_Vulkan.h"

#include "Enums_Vulkan.h"
#include "Platform/PlatformLogcat.h"
#include "Platform/RHI/Image.h"

VkComponentMapping platform::rhi::vulkan::FromComponentMapping(const ComponentMapping& mapping)
{
    VkComponentMapping vk_mapping;
    vk_mapping.a = RHIComponentMappingElementToVkComponentSwizzle(mapping.r);
    vk_mapping.r = RHIComponentMappingElementToVkComponentSwizzle(mapping.r);
    vk_mapping.g = RHIComponentMappingElementToVkComponentSwizzle(mapping.g);
    vk_mapping.b = RHIComponentMappingElementToVkComponentSwizzle(mapping.b);
    return vk_mapping;
}

platform::rhi::vulkan::ImageView_Vulkan::ImageView_Vulkan(const ImageViewDesc& desc) : ImageView(desc)
{
    const auto img        = desc_.image;
    auto       img_handle = static_cast<VkImage>(img->GetNativeHandle());
    Assert(logcat::Platform_RHI_Vulkan, img, "Image cannot be nullptr when creating a ImageView.");
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = img_handle;
    view_info.components = FromComponentMapping(desc_.component_mapping);
    view_info.format = RHIFormatToVkFormat(desc_.format);
    view_info.subresourceRange.aspectMask = RHIImageAspectToVkImageAspect(desc_.subresource_range.aspect_mask);
}
