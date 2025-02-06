//
// Created by Echo on 25-2-6.
//

#include "CoordConversion.h"

#include "Platform/RHI/GfxContext.h"

using namespace func;
using namespace ui;
using namespace impl;
using namespace core;
using namespace platform;
using namespace rhi;

Vector2 impl::operator|(const Vector2 pos, const UIPos2NDC_Impl) {
  switch (GetGfxContextRef().GetAPI()) {
  case GraphicsAPI::Vulkan:
  default:
    return {2 * pos.x - 1, 1 - 2 * pos.y};
  }
}