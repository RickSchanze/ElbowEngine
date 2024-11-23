/**
 * @file RHIEnums_Vulkan.h
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#pragma once

#include "Platform/RHI/RHIEnums.h"
#include "vulkan/vulkan.h"

VkFormat RHIFormatToVkFormat(platform::RHIFormat format);
platform::RHIFormat VkFormatToRHIFormat(VkFormat format);

VkColorSpaceKHR RHIColorSpaceToVkColorSpace(platform::RHIColorSpace color_space);
platform::RHIColorSpace VkColorSpaceToRHIColorSpace(VkColorSpaceKHR color_space);

VkPresentModeKHR RHIPresentModeToVkPresentMode(platform::RHIPresentMode present_mode);
platform::RHIPresentMode VkPresentModeToRHIPresentMode(VkPresentModeKHR present_mode);

VkSampleCountFlagBits RHISampleCountToVkSampleCount(platform::RHISampleCount sample_count);
platform::RHISampleCount VkSampleCountToRHISampleCount(VkSampleCountFlagBits sample_count);
