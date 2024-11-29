/**
 * @file ImageView_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-25
 * @brief 
 */

#include "ImageView_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"

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
    handle_ = GetVulkanGfxContext()->CreateImageView(desc_);
}

platform::rhi::vulkan::ImageView_Vulkan::~ImageView_Vulkan()
{
    GetVulkanGfxContext()->DestroyImageView(handle_);
}
