/**
 * @file TextureFormat.h
 * @author Echo 
 * @Date 24-10-20
 * @brief 
 */

#pragma once
#include "RHI/TextureFormat.h"
#include "vulkan/vulkan.hpp"

namespace rhi::vulkan
{
vk::Format GetTextureFormat(TextureFormat format);
}
