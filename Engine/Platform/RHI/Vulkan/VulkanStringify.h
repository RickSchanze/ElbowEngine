/**
 * @file VulkanStringify.h
 * @author Echo 
 * @Date 24-4-27
 * @brief Vulkan常用类型ToString
 */

#pragma once
#include "Core/Base/EString.h"
#include "vulkan/vulkan.hpp"

class VulkanStringify {
public:
    // TODO: 基于反射Type的ToString
    static core::String ToString(const vk::Extent2D& InExtent);
};
