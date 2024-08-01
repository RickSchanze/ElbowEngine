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
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>

RTTR_REGISTRATION
{
    rttr::registration::class_<Rotator>("Rotator")
        .constructor<>()
        .property("yaw", &Rotator::yaw)
        .property("roll", &Rotator::roll)
        .property("pitch", &Rotator::pitch);

    rttr::registration::class_<Vector3>("Vector3").constructor<>().property("x", &Vector3::x).property("y", &Vector3::y).property("z", &Vector3::z);

    rttr::registration::class_<Transform>("Transform")
        .constructor<>()
        .property("location", &Transform::position)
        .property("rotation", &Transform::rotation)
        .property("scale", &Transform::scale);

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
        L"Transform[location: <{}>, rotation: <{}>, scale: <{}>]", StringUtils::ToString(position), rotation.ToString(), StringUtils::ToString(scale)
    );
}

glm::mat4 Transform::ToMat4() const
{
    auto mat = glm::mat4(1.0f);
    mat      = translate(mat, position);
    mat      = rotate(mat, glm::radians(rotation.yaw), Constant::UpVector);
    mat      = rotate(mat, glm::radians(rotation.pitch), GetRightVector());
    mat      = rotate(mat, glm::radians(rotation.roll), GetForwardVector());
    mat      = glm::scale(mat, scale);
    return mat;
}

glm::mat4 Transform::ToGPUMat4() const
{
    auto rtn = glm::mat4(1.0f);
    rtn[0] = Math::ToVector4(position);
    return rtn;
}
