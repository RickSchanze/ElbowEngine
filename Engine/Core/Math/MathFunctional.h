//
// Created by Echo on 25-2-6.
//

#pragma once
#include "MathTypes.h"

namespace core {

namespace impl {
struct ToVector2_Impl {};
Vector2 operator|( Vector3 v, ToVector2_Impl);
} // namespace impl

constexpr inline auto ToVector2 = impl::ToVector2_Impl{};

} // namespace core