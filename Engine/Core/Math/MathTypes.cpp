/**
 * @file MathTypes.cpp
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#include "MathTypes.h"

#include "Utils/StringUtils.h"

#include <format>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

RTTR_REGISTRATION
{
    rttr::registration::class_<Rotator>("Rotator")
        .constructor<>()
        .property("yaw", &Rotator::yaw)
        .property("roll", &Rotator::roll)
        .property("pitch", &Rotator::pitch);

    rttr::registration::class_<Vector3>("Vector3")
        .constructor<>()
        .property("x", &Vector3::x)
        .property("y", &Vector3::y)
        .property("z", &Vector3::z);

    rttr::registration::class_<Transform>("Transform")
        .constructor<>()
        .property("location", &Transform::location)
        .property("rotation", &Transform::rotation)
        .property("scale", &Transform::scale);
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

Vector3 Transform::GetForwardVector() const
{
    return rotation.GetForwardVector();
}

Vector3 Transform::GetUpVector() const
{
    return rotation.GetUpVector();
}

Vector3 Transform::GetRightVector() const
{
    return rotation.GetRightVector();
}

String Transform::ToString() const
{
    return std::format(
        L"Transform[location: <{}>, rotation: <{}>, scale: <{}>]",
        StringUtils::ToString(location),
        rotation.ToString(),
        StringUtils::ToString(scale)
    );
}
