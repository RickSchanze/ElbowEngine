//
// Created by Echo on 25-2-6.
//

#pragma once
#include "Core/Math/MathTypes.h"

namespace func::ui {

namespace impl {
struct UIPos2NDC_Impl {};
core::Vector2 operator|(core::Vector2 pos, const UIPos2NDC_Impl);
} // namespace impl

inline constexpr auto UIPos2NDC = impl::UIPos2NDC_Impl{};

} // namespace func::ui
