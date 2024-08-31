/**
 * @file MathTypes.h
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "Interface/IStringify.h"

#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>

#if USE_IMGUI
#    include <imgui.h>
#endif

struct Size2D : IStringify
{
    int32_t width;
    int32_t height;

    Size2D(const int32_t w, const int32_t h) : width(w), height(h) {}

    String ToString() const override;
};


typedef glm::vec3 Vector3;

struct Vector2
{
    operator glm::vec2();

#if USE_IMGUI
    operator ImVec2();
#endif

    Vector2 operator+(const Vector2& other);

    Vector2 operator*=(const float scalar);

    float x;
    float y;
};

typedef glm::vec4 Vector4;
typedef glm::mat4 Matrix4x4;
typedef glm::quat Quaternion;

namespace Constant
{
constexpr Vector3 ZeroVector    = {0, 0, 0};
constexpr Vector3 OneVector     = {1, 1, 1};
constexpr Vector3 ForwardVector = {0, 0, 1};
constexpr Vector3 BackVector    = {0, 0, -1};
constexpr Vector3 RightVector   = {1, 0, 0};
constexpr Vector3 LeftVector    = {-1, 0, 0};
constexpr Vector3 UpVector      = {0, 1, 0};
constexpr Vector3 DownVector    = {0, -1, 0};

constexpr float PI = 3.14159265358979323846f;
}   // namespace Constant

struct Rotator
{
    float yaw   = 0;
    float roll  = 0;
    float pitch = 0;

    Rotator(float yaw = 0, float roll = 0, float pitch = 0) : yaw(yaw), roll(roll), pitch(pitch) {}
    Rotator(const Vector3 v) : yaw(v.x), roll(v.y), pitch(v.z) {}

    Vector3 GetForwardVector() const;
    Vector3 GetUpVector() const;
    Vector3 GetRightVector() const;
    String  ToString() const;

    bool operator==(const Rotator&) const;
    bool operator!=(const Rotator&) const;

    Rotator& operator=(const Vector3& v);
    Rotator  operator-(const Rotator& other) const;
    Rotator  operator+(const Rotator& other) const;

    Vector3 ToVector3() const { return {pitch, yaw, roll}; }

    Rotator& operator+=(const Rotator& other);
    Rotator& operator-=(const Rotator& other);

    /**
     * 将角度表示的自己转变为弧度表示
     * @return
     */
    Rotator ToRotatorRadian() const;
};

struct Color
{
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 1.f;

public:
    static Color Black() { return {0, 0, 0, 1}; }
    static Color White() { return {1, 1, 1, 1}; }
    static Color Red() { return {1, 0, 0, 1}; }
    static Color Green() { return {0, 1, 0, 1}; }
    static Color Blue() { return {0, 0, 1, 1}; }
    static Color Yellow() { return {1, 1, 0, 1}; }
    static Color Warning() { return {1, 1, 0, 1}; }
    static Color Error() { return {1, 0, 0, 1}; }
    static Color Info() { return {0, 1, 1, 1}; }
    static Color SkyBlue() { return {0.4f, 0.6f, 1.f, 1.f}; }
    static Color Invalid() { return {-1, -1, -1, -1}; }

    bool IsValid() const;

    Color operator*(const Color& other)const;
    Color operator*(const float scalar)const;

#if USE_IMGUI
    Color() = default;
    Color(ImVec4 color) : r(color.x), g(color.y), b(color.z), a(color.w) {}
    Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {}
    operator ImVec4() { return {r, g, b, a}; }
    explicit operator ImU32() { return ImGui::GetColorU32((ImVec4)(*this)); }
#endif

    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;
};

inline Matrix4x4 GetMatrix4x4Identity()
{
    static auto identity = glm::mat4(1.0f);
    return identity;
}
