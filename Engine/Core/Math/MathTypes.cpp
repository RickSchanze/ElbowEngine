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

String Size2D::ToString() {
    return std::format(L"Width: {}, Height:{}", Width, Height);
}


FVector3 FRotator::GetForwardVector() const {
    FVector3 Forward;
    Forward.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Forward.y = sin(glm::radians(Pitch));
    Forward.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    return normalize(Forward);
}

FVector3 FRotator::GetUpVector() const {
    return cross(GetRightVector(), GetForwardVector());
}

FVector3 FRotator::GetRightVector() const {
    return cross(GetForwardVector(), Constant::UpVector);
}
