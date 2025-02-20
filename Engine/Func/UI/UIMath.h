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

inline Int32 GetPaddingLeft(core::Vector4I padding) { return padding.x; }
inline Int32 GetPaddingRight(core::Vector4I padding) { return padding.z; }
inline Int32 GetPaddingTop(core::Vector4I padding) { return padding.y; }
inline Int32 GetPaddingBottom(core::Vector4I padding) { return padding.w; }

} // namespace func::ui
