//
// Created by Echo on 25-2-6.
//

#pragma once
#include "MathTypes.h"

namespace core {

namespace impl {
struct ToVector2_Impl {};
inline Vector2 operator|(Vector3 v, ToVector2_Impl) { return {v.x, v.y}; }
inline Vector2 operator|(Vector2I v, ToVector2_Impl) { return {(Float)v.x, (Float)v.y}; }

struct ToVector3_Impl {};
inline Vector3 operator|(Vector2 v, ToVector3_Impl) { return {v.x, v.y, 0.f}; }

struct DivideNum_Impl {
  Float num;
};
inline Vector3 operator|(Vector3 v, DivideNum_Impl div) { return {v.x / div.num, v.y / div.num, v.z / div.num}; }

struct DivideVec3_Impl {
  Vector3 v;
};
inline Vector3 operator|(const Vector3& v, DivideVec3_Impl div) { return {v.x / div.v.x, v.y / div.v.y, v.z / div.v.z}; }

struct ClampNum_Impl {
  Float min_, max_;
};
inline Vector3 operator|(Vector3 v, ClampNum_Impl clamp) {
  return {std::clamp(v.x, clamp.min_, clamp.max_), std::clamp(v.y, clamp.min_, clamp.max_),
          std::clamp(v.z, clamp.min_, clamp.max_)};
}
inline Vector2 operator|(Vector2 v, ClampNum_Impl clamp) {
  return {std::clamp(v.x, clamp.min_, clamp.max_), std::clamp(v.y, clamp.min_, clamp.max_)};
}

} // namespace impl

constexpr inline auto ToVector2 = impl::ToVector2_Impl{};
constexpr inline auto ToVector3 = impl::ToVector3_Impl{};

constexpr impl::DivideNum_Impl Divide(const Float num) { return {num}; }
constexpr impl::DivideVec3_Impl Divide(const Vector3 v) { return {v}; }
constexpr impl::ClampNum_Impl Clamp(const Float min_ = 0, const Float max_ = 1) { return {min_, max_}; }

} // namespace core