/**
 * @file VulkanUtils.h
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#pragma once
#include "VulkanCommon.h"

namespace rhi::vulkan
{
class VulkanUtils
{
public:
    static size_t GetDynamicUniformModelAlignment();
};
}   // namespace rhi::vulkan
