/**
 * @file VulkanStringify.h
 * @author Echo 
 * @Date 24-4-27
 * @brief Vulkan常用类型ToString
 */

#pragma once
#include "CoreDef.h"
#include "vulkan/vulkan.hpp"

class VulkanStringify {
public:
    static String ToString(const vk::Extent2D& InExtent);
};
