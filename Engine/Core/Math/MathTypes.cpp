/**
 * @file MathTypes.cpp
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#include "MathTypes.h"

#include <format>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

RTTR_REGISTRATION {
    rttr::registration::class_<Rotator>("FRotator")
        .constructor<>()
        .property("Yaw", &Rotator::yaw)
        .property("Roll", &Rotator::roll)
        .property("Pitch", &Rotator::pitch);

    rttr::registration::class_<Vector3>("FVector3")
        .constructor<>()
        .property("X", &Vector3::x)
        .property("Y", &Vector3::y)
        .property("z", &Vector3::z);
}

String Size2D::ToString() {
    return std::format(L"Width: {}, Height:{}", width, height);
}


Vector3 Rotator::GetForwardVector() const {
    Vector3 Forward;
    Forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    Forward.y = sin(glm::radians(pitch));
    Forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return normalize(Forward);
}

Vector3 Rotator::GetUpVector() const {
    return cross(GetRightVector(), GetForwardVector());
}

Vector3 Rotator::GetRightVector() const {
    return cross(GetForwardVector(), Constant::UpVector);
}
