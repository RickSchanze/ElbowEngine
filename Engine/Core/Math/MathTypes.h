/**
 * @file MathTypes.h
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#pragma once

#include "Core/Base/EString.h"
#include "Core/CoreDef.h"
#include "Core/Reflection/MetaInfoMacro.h"
#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

#include GEN_HEADER("Core.MathTypes.generated.h")
#include "Core/Reflection/MetaInfoManager.h"

namespace core
{
class Archive;

struct STRUCT() Size2D
{
    GENERATED_STRUCT(Size2D)

    PROPERTY()
    int32_t width;

    PROPERTY()
    int32_t height;

    Size2D(const int32_t w, const int32_t h) : width(w), height(h) {}

    Size2D() = default;

    [[nodiscard]] core::String ToString() const;
};


struct STRUCT() Vector2
{
    GENERATED_STRUCT(Vector2)

    Vector2() = default;
    Vector2(float x, float y) : x(x), y(y) {}

    operator glm::vec2() const;
    operator glm::vec3() const { return {x, y, 0}; }
    operator glm::vec4() const { return {x, y, 0, 0}; }

#ifdef USE_IMGUI
    operator ImVec2() const;
#endif

    Vector2 operator+(const Vector2& other) const;

    Vector2 operator*=(float scalar) const;
    Vector2 operator*(float x) const;

    PROPERTY()
    float x;

    PROPERTY()
    float y;
};

typedef glm::vec3 Vector3;
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

struct STRUCT() Rotator
{
    GENERATED_STRUCT(Rotator)
public:
    PROPERTY()
    float yaw = 0;

    PROPERTY()
    float roll = 0;

    PROPERTY()
    float pitch = 0;

    Rotator(float yaw = 0, float roll = 0, float pitch = 0) : yaw(yaw), roll(roll), pitch(pitch) {}
    Rotator(const Vector3 v) : yaw(v.x), roll(v.y), pitch(v.z) {}

    [[nodiscard]] Vector3      GetForwardVector() const;
    [[nodiscard]] Vector3      GetUpVector() const;
    [[nodiscard]] Vector3      GetRightVector() const;
    [[nodiscard]] core::String ToString() const;

    bool operator==(const Rotator&) const;
    bool operator!=(const Rotator&) const;

    Rotator& operator=(const Vector3& v);
    Rotator  operator-(const Rotator& other) const;
    Rotator  operator+(const Rotator& other) const;

    [[nodiscard]] Vector3 ToVector3() const { return {pitch, yaw, roll}; }

    Rotator& operator+=(const Rotator& other);
    Rotator& operator-=(const Rotator& other);

    /**
     * 将角度表示的自己转变为弧度表示
     * @return
     */
    [[nodiscard]] Rotator ToRotatorRadian() const;
};

struct STRUCT() Color
{
    GENERATED_STRUCT(Color)
public:
    PROPERTY()
    float r = 0;

    PROPERTY()
    float g = 0;

    PROPERTY()
    float b = 0;

    PROPERTY()
    float a = 1.f;

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

    /** 讲一个0-1的表示颜色的值转换为0-255的值 */
    static uint8_t FloatToByte(float value);

    [[nodiscard]] bool IsValid() const;

    Color operator*(const Color& other) const;
    Color operator*(float scalar) const;

    /** 转换Int带alpha通道 */
    [[nodiscard]] uint32_t ToUInt() const;
    /** 转换为没有alpha通道的Int */
    [[nodiscard]] uint32_t ToUIntNoAlpha() const;

#ifdef USE_IMGUI
    Color() = default;
    Color(const ImVec4 color) : r(color.x), g(color.y), b(color.z), a(color.w) {}
    Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {}
    operator ImVec4() const { return {r, g, b, a}; }
    explicit operator ImU32() const { return ImGui::GetColorU32((ImVec4)(*this)); }
#endif

    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;
};

inline Matrix4x4 GetMatrix4x4Identity()
{
    static auto identity = glm::mat4(1.0f);
    return identity;
}
}
