/**
 * @file RHIEnums_Vulkan.h
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
