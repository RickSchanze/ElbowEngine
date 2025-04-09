//
// Created by Echo on 2025/4/9.
//

#pragma once
#include <glm/vec3.hpp>

#include "Types.hpp"

namespace impl {
    struct Ext_ToGLMVec3 {};
    struct Ext_ToMatrix4x4f {};

    struct Ext_ToVector4f {};
    struct Ext_ToGLMVec4 {};
} // namespace impl

inline constexpr impl::Ext_ToGLMVec3 ToGLMVec3{};
inline constexpr impl::Ext_ToGLMVec4 ToGLMVec4{};
inline constexpr impl::Ext_ToMatrix4x4f ToMatrix4x4f{};
inline constexpr impl::Ext_ToVector4f ToVector4f{};

inline glm::vec3 operator|(const Vector3f v, impl::Ext_ToGLMVec3) { return glm::vec3(v.x, v.y, v.z); }
inline glm::vec4 operator|(const Vector4f v, impl::Ext_ToGLMVec4) { return glm::vec4(v.x, v.y, v.z, v.w); }
inline Vector4f operator|(const glm::vec4 &v, impl::Ext_ToVector4f) { return Vector4f(v.x, v.y, v.z, v.w); }
inline Matrix4x4f operator|(const glm::mat4 &m, impl::Ext_ToMatrix4x4f) {
    Matrix4x4f result;
    result.col1 = m[0] | ToVector4f;
    result.col2 = m[1] | ToVector4f;
    result.col3 = m[2] | ToVector4f;
    result.col4 = m[3] | ToVector4f;
    return result;
}
