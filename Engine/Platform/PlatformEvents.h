/**
 * @file PlatformEvents.h
 * @author Echo 
 * @Date 24-6-11
 * @brief 
 */

#pragma once

#include "Event/Event.h"

namespace rhi::vulkan
{
class Texture;
}

namespace Platform
{

struct RequestLoadEngineDefaultLackTextureEvent
    : TEvent<
          rhi::vulkan::Texture** /** OutLackTexture*/,
          rhi::vulkan::ImageView** /** OutLackTextureView */>
{
};

inline RequestLoadEngineDefaultLackTextureEvent OnRequestLoadDefaultLackTexture;

}   // namespace Platform
