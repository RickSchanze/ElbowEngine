/**
 * @file Enums_Vulkan.h
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#pragma once

#include "Platform/RHI/Enums.h"
#include "vulkan/vulkan.h"

VkFormat              RHIFormatToVkFormat(platform::rhi::Format format);
platform::rhi::Format VkFormatToRHIFormat(VkFormat format);

VkColorSpaceKHR           RHIColorSpaceToVkColorSpace(platform::rhi::ColorSpace color_space);
platform::rhi::ColorSpace VkColorSpaceToRHIColorSpace(VkColorSpaceKHR color_space);

VkPresentModeKHR           RHIPresentModeToVkPresentMode(platform::rhi::PresentMode present_mode);
platform::rhi::PresentMode VkPresentModeToRHIPresentMode(VkPresentModeKHR present_mode);

VkSampleCountFlagBits      RHISampleCountToVkSampleCount(platform::rhi::SampleCount sample_count);
platform::rhi::SampleCount VkSampleCountToRHISampleCount(VkSampleCountFlagBits sample_count);

/**
 * @param aspect 由ImageAspect组成的bitmask, 类型int
 * @return 由VkImageAspectFlagBits组成的bitmask, 类型VkImageAspectFlags(uint32_t)
 */
VkImageAspectFlags RHIImageAspectToVkImageAspect(/** ImageAspect */ int aspect);
/**
 * @param aspect 由VkImageAspectFlagBits组成的bitmask, 类型VkImageAspectFlags(uint32_t)
 * @return 由ImageAspect组成的bitmask, 类型int
 */
int                VkImageAspectToRHIImageAspect(VkImageAspectFlags aspect);

platform::rhi::ComponentMappingElement VkComponentSwizzleToRHIComponentMappingElement(VkComponentSwizzle swizzle);
VkComponentSwizzle                     RHIComponentMappingElementToVkComponentSwizzle(platform::rhi::ComponentMappingElement swizzle);

VkImageViewType              RHIImageViewTypeToVkImageViewType(platform::rhi::ImageViewType type);
platform::rhi::ImageViewType VkImageViewTypeToRHIImageViewType(VkImageViewType type);
