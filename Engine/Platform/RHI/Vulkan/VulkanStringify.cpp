/**
 * @file VulkanStringifiy.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#include "VulkanStringify.h"
#include "vulkan/vulkan_to_string.hpp"
core::String VulkanStringify::ToString(const vk::Extent2D& InExtent) {
    return std::format("(Width={}, Height={})", InExtent.width, InExtent.height);
}

