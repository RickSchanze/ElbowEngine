/**
 * @file MathTypes.cpp
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#include "MathTypes.h"

#include "Math.h"
#include "Utils/StringUtils.h"

#include <format>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/trigonometric.hpp>

RTTR_REGISTRATION
{
    rttr::registration::class_<Rotator>("Rotator")
        .constructor<>()
        .property("yaw", &Rotator::yaw)
        .property("roll", &Rotator::roll)
        .property("pitch", &Rotator::pitch);

    rttr::registration::class_<Vector3>("Vector3").constructor<>().property("x", &Vector3::x).property("y", &Vector3::y).property("z", &Vector3::z);

    rttr::registration::class_<Color>("Color")
        .constructor<>()
        .property("r", &Color::r)
        .property("g", &Color::g)
        .property("b", &Color::b)
        .property("a", &Color::a);
}

String Size2D::ToString() const
{
    return std::format(L"Width: {}, Height:{}", width, height);
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

String Rotator::ToString() const
{
    return std::format(L"Yaw: {}, Pitch: {}, Roll: {}", yaw, pitch, roll);
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
    rtn.yaw   = Math::Clamp(yaw - other.yaw, -180.0f, 180.0f);
    rtn.pitch = Math::Clamp(pitch - other.pitch, -180.0f, 180.0f);
    rtn.roll  = Math::Clamp(roll - other.roll, -180.0f, 180.0f);
    return rtn;
}

Rotator Rotator::operator+(const Rotator& other) const
{
    Rotator rtn;
    rtn.yaw   = Math::Clamp(yaw + other.yaw, -180.0f, 180.0f);
    rtn.pitch = Math::Clamp(pitch + other.pitch, -180.0f, 180.0f);
    rtn.roll  = Math::Clamp(roll + other.roll, -180.0f, 180.0f);
    return rtn;
}

Rotator& Rotator::operator+=(const Rotator& other)
{
    yaw   = Math::Clamp(yaw + other.yaw, -180.0f, 180.0f);
    pitch = Math::Clamp(pitch + other.pitch, -180.0f, 180.0f);
    roll  = Math::Clamp(roll + other.roll, -180.0f, 180.0f);
    return *this;
}

Rotator& Rotator::operator-=(const Rotator& other)
{
    yaw   = Math::Clamp(yaw - other.yaw, -180.0f, 180.0f);
    pitch = Math::Clamp(pitch - other.pitch, -180.0f, 180.0f);
    roll  = Math::Clamp(roll - other.roll, -180.0f, 180.0f);
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

bool Color::operator==(const Color& other) const
{
    return Math::ApproximatelyEqual(r, other.r) && Math::ApproximatelyEqual(g, other.g) && Math::ApproximatelyEqual(b, other.b) &&
           Math::ApproximatelyEqual(a, other.a);
}

bool Color::operator!=(const Color& other) const
{
    return !(*this == other);
}
