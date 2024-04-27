/**
 * @file VulkanStringifiy.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "vulkan/vulkan.hpp"

class VulkanStringifiy {
public:
    static String ToString(const vk::Extent2D& InExtent);
};
