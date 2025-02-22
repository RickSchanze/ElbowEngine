//
// Created by Echo on 25-2-6.
//

#pragma once
#include "Core/Math/MathTypes.h"

namespace func::ui {

namespace impl {
struct UIPos2NDC_Impl {};
core::Vector2 operator|(core::Vector2 pos, UIPos2NDC_Impl);
} // namespace impl

inline constexpr auto UIPos2NDC = impl::UIPos2NDC_Impl{};

inline Int32 ExtractPaddingLeft(core::Vector4I padding) { return padding.x; }
inline Int32 ExtractPaddingRight(core::Vector4I padding) { return padding.z; }
inline Int32 ExtractPaddingTop(core::Vector4I padding) { return padding.y; }
inline Int32 ExtractPaddingBottom(core::Vector4I padding) { return padding.w; }

core::Vector2 MathCoordToScreenCoord(core::Vector2 math_coord);
core::Vector2 ExtractLeftTopScreenSpace(core::Rect2D);
core::Vector2 ExtractRightBottomScreenSpace(core::Rect2D);
core::Vector2 ExtractLeftBottomScreenSpace(core::Rect2D);
core::Vector2 ExtractRightTopScreenSpace(core::Rect2D);
} // namespace func::ui
