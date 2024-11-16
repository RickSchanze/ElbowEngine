/**
 * @file TextureFormatVulkan'.h
 * @author Echo 
 * @Date 24-10-20
 * @brief 
 */

#pragma once
#include "Platform/RHI/TextureFormat.h"
#include "vulkan/vulkan.hpp"

namespace rhi::vulkan
{
vk::Format GetTextureFormat(TextureFormat format);
}
