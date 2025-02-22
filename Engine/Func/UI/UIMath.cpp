//
// Created by Echo on 25-2-6.
//

#include "UIMath.h"

#include "Core/Math/MathPipe.h"
#include "Platform/RHI/GfxContext.h"

using namespace func;
using namespace ui;
using namespace impl;
using namespace core;
using namespace platform;
using namespace rhi;

Vector2 ui::impl::operator|(const Vector2 pos, const UIPos2NDC_Impl) {
  switch (GetGfxContextRef().GetAPI()) {
  case GraphicsAPI::Vulkan:
  default:
    return Vector2{2 * pos.x - 1, 1 - 2 * pos.y} | Clamp(-1.f, 1.f);
  }
}

core::Vector2 ui::MathCoordToScreenCoord(core::Vector2 math_coord) {
  core::Vector2 screen_coord;
  screen_coord.x = math_coord.x;
  screen_coord.y = 1 - math_coord.y;
  return screen_coord;
}

core::Vector2 ui::ExtractLeftTopScreenSpace(core::Rect2D screen) { return {screen.position.x, screen.position.y}; }

core::Vector2 ui::ExtractRightBottomScreenSpace(core::Rect2D screen) {
  return {screen.position.x + screen.size.x, screen.position.y + screen.size.y};
}

core::Vector2 ui::ExtractLeftBottomScreenSpace(core::Rect2D screen) {
  return {screen.position.x, screen.position.y + screen.size.y};
}

core::Vector2 ui::ExtractRightTopScreenSpace(core::Rect2D screen) {
  return {screen.position.x + screen.size.x, screen.position.y};
}