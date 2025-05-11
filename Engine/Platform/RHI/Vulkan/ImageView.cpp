//
// Created by Echo on 2025/3/25.
//

#include "ImageView.hpp"

#include "Enums.hpp"
#include "GfxContext.hpp"

using namespace NRHI;

VkComponentMapping NRHI::FromComponentMapping(const ComponentMapping &mapping) {
    VkComponentMapping vk_mapping;
    vk_mapping.a = RHIComponentMappingElementToVkComponentSwizzle(mapping.a);
    vk_mapping.r = RHIComponentMappingElementToVkComponentSwizzle(mapping.r);
    vk_mapping.g = RHIComponentMappingElementToVkComponentSwizzle(mapping.g);
    vk_mapping.b = RHIComponentMappingElementToVkComponentSwizzle(mapping.b);
    return vk_mapping;
}

ImageView_Vulkan::ImageView_Vulkan(const ImageViewDesc &desc) : ImageView(desc) { handle_ = GetVulkanGfxContext()->CreateImageView_VK(desc_); }

ImageView_Vulkan::~ImageView_Vulkan() {
    GetVulkanGfxContext()->DestroyImageView_VK(handle_);
    handle_ = nullptr;
}
