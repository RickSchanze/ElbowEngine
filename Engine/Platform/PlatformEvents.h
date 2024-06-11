/**
 * @file PlatformEvents.h
 * @author Echo 
 * @Date 24-6-11
 * @brief 
 */

#pragma once

#include "Event/Event.h"

namespace RHI::Vulkan
{
class Texture;
}

namespace Platform
{

struct RequestLoadEngineDefaultLackTextureEvent
    : TEvent<
          RHI::Vulkan::Texture** /** OutLackTexture*/,
          RHI::Vulkan::ImageView** /** OutLackTextureView */>
{
};

inline RequestLoadEngineDefaultLackTextureEvent OnRequestLoadDefaultLackTexture;

}   // namespace Platform
