/**
 * @file MathTypes.cpp
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#include "MathTypes.h"

#include "Math.h"

#include <format>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "Serialization/Archive.h"


#include <glm/trigonometric.hpp>

namespace core
{
RTTR_REGISTRATION
{
    rttr::registration::class_<Rotator>("core::Rotator")
        .constructor<>()
        .property("yaw", &Rotator::yaw)
        .property("roll", &Rotator::roll)
        .property("pitch", &Rotator::pitch);

    rttr::registration::class_<Vector3>("core::Vector3")
        .constructor<>()
        .property("x", &Vector3::x)
        .property("y", &Vector3::y)
        .property("z", &Vector3::z);

    rttr::registration::class_<Color>("core::Color")
        .constructor<>()
        .property("r", &Color::r)
        .property("g", &Color::g)
        .property("b", &Color::b)
        .property("a", &Color::a);
}

core::String Size2D::ToString() const
{
    return std::format("Width: {}, Height:{}", width, height);
}

Vector2::operator glm::vec<2, float>() const
{
    return {x, y};
}

#ifdef USE_IMGUI
Vector2::operator ImVec2() const
{
    return {x, y};
}
#endif

Vector2 Vector2::operator+(const Vector2& other) const
{
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator*=(const float scalar) const
{
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator*(float x) const
{
    return {this->x * x, y * x};
}


Vector3 Rotator::GetForwardVector() const
{
    Vector3 Forward;
    Forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    Forward.y = sin(glm::radians(pitch));
    Forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return normalize(Forward);
}

Vector3 Rotator::GetUpVector() const
{
    return cross(GetRightVector(), GetForwardVector());
}

Vector3 Rotator::GetRightVector() const
{
    return cross(GetForwardVector(), Constant::UpVector);
}

core::String Rotator::ToString() const
{
    return std::format("Yaw: {}, Pitch: {}, Roll: {}", yaw, pitch, roll);
}

bool Rotator::operator==(const Rotator& other) const
{
    return Math::ApproximatelyEqual(yaw, other.yaw) && Math::ApproximatelyEqual(pitch, other.pitch) && Math::ApproximatelyEqual(roll, other.roll);
}

bool Rotator::operator!=(const Rotator& other) const
{
    return !(*this == other);
}

Rotator& Rotator::operator=(const Vector3& v)
{
    yaw   = v.x;
    pitch = v.y;
    roll  = v.z;
    return *this;
}

Rotator Rotator::operator-(const Rotator& other) const
{
    Rotator rtn;
    rtn.yaw   = Math::RoundAngle(yaw - other.yaw, -180.0f, 180.0f);
    rtn.pitch = Math::RoundAngle(pitch - other.pitch, -180.0f, 180.0f);
    rtn.roll  = Math::RoundAngle(roll - other.roll, -180.0f, 180.0f);
    return rtn;
}

Rotator Rotator::operator+(const Rotator& other) const
{
    Rotator rtn;
    rtn.yaw   = Math::RoundAngle(yaw + other.yaw, -180.0f, 180.0f);
    rtn.pitch = Math::RoundAngle(pitch + other.pitch, -180.0f, 180.0f);
    rtn.roll  = Math::RoundAngle(roll + other.roll, -180.0f, 180.0f);
    return rtn;
}

Rotator& Rotator::operator+=(const Rotator& other)
{
    yaw   = Math::RoundAngle(yaw + other.yaw);
    pitch = Math::RoundAngle(pitch + other.pitch);
    roll  = Math::RoundAngle(roll + other.roll);
    return *this;
}

Rotator& Rotator::operator-=(const Rotator& other)
{
    yaw   = Math::RoundAngle(yaw - other.yaw);
    pitch = Math::RoundAngle(pitch - other.pitch);
    roll  = Math::RoundAngle(roll - other.roll);
    return *this;
}

Rotator Rotator::ToRotatorRadian() const
{
    Rotator r;
    r.roll  = Math::Radians(roll);
    r.pitch = Math::Radians(pitch);
    r.yaw   = Math::Radians(yaw);
    return r;
}

uint8_t Color::FloatToByte(float value)
{
    return static_cast<uint8_t>(std::round(value * 255.0f));
}

bool Color::IsValid() const
{
    return r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1 && a >= 0 && a <= 1;
}

Color Color::operator*(const Color& other) const
{
    return {r * other.r, g * other.g, b * other.b, a * other.a};
}

Color Color::operator*(const float scalar) const
{
    return {r * scalar, g * scalar, b * scalar, a * scalar};
}

uint32_t Color::ToUInt() const
{
    return FloatToByte(a) << 24 | FloatToByte(r) << 16 | FloatToByte(g) << 8 | FloatToByte(b);
}

uint32_t Color::ToUIntNoAlpha() const
{
    return FloatToByte(r) << 16 | FloatToByte(g) << 8 | FloatToByte(b);
}

bool Color::operator==(const Color& other) const
{
    return Math::ApproximatelyEqual(r, other.r) && Math::ApproximatelyEqual(g, other.g) && Math::ApproximatelyEqual(b, other.b) &&
           Math::ApproximatelyEqual(a, other.a);
}

bool Color::operator!=(const Color& other) const
{
    return !(*this == other);
}

Archive& operator<<(Archive& ar, const Vector2& v)
{
    // Assert(Archive.Serialization, ar.IsSerializing(), "请在序列化模式使用此函数");
    ar << Archive::InputType::MapStart;
    ar << Archive::InputType::Key << "x" << Archive::InputType::Value << v.x;
    ar << Archive::InputType::Key << "y" << Archive::InputType::Value << v.y;
    ar << Archive::InputType::MapEnd;
    return ar;
}

Archive& operator<<(Archive& ar, const Vector3& v)
{
    // Assert(Archive.Serialization, ar.IsSerializing(), "请在序列化模式使用此函数");
    ar << Archive::InputType::MapStart;
    ar << Archive::InputType::Key << "x" << Archive::InputType::Value << v.x;
    ar << Archive::InputType::Key << "y" << Archive::InputType::Value << v.y;
    ar << Archive::InputType::Key << "z" << Archive::InputType::Value << v.z;
    ar << Archive::InputType::MapEnd;
    return ar;
}

Archive& operator<<(Archive& ar, const Vector4& v)
{
    // Assert(Archive.Serialization, ar.IsSerializing(), "请在序列化模式使用此函数");
    ar << Archive::InputType::MapStart;
    ar << Archive::InputType::Key << "x" << Archive::InputType::Value << v.x;
    ar << Archive::InputType::Key << "y" << Archive::InputType::Value << v.y;
    ar << Archive::InputType::Key << "z" << Archive::InputType::Value << v.z;
    ar << Archive::InputType::Key << "w" << Archive::InputType::Value << v.w;
    ar << Archive::InputType::MapEnd;
    return ar;
}

Archive& operator<<(Archive& ar, const Color& q)
{
    // Assert(Archive.Serialization, ar.IsSerializing(), "请在序列化模式使用此函数");
    ar << Archive::InputType::MapStart;
    ar << Archive::InputType::Key << "r" << Archive::InputType::Value << q.r;
    ar << Archive::InputType::Key << "g" << Archive::InputType::Value << q.g;
    ar << Archive::InputType::Key << "b" << Archive::InputType::Value << q.b;
    ar << Archive::InputType::Key << "a" << Archive::InputType::Value << q.a;
    ar << Archive::InputType::MapEnd;
    return ar;
}
}
