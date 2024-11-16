/**
 * @file PlatformEvents.h
 * @author Echo 
 * @Date 24-6-11
 * @brief 
 */

#pragma once

#include "Core/Event/Event.h"

namespace rhi::vulkan
{
class ImageView;
class Texture;
}

namespace platform
{

DECLARE_EVENT(RequestLoadEngineDefaultLackTextureEvent, void, rhi::vulkan::Texture**, rhi::vulkan::ImageView**);

inline RequestLoadEngineDefaultLackTextureEvent OnRequestLoadDefaultLackTexture;

}   // namespace platform
