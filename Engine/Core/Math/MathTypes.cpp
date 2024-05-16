/**
 * @file MathTypes.cpp
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#include "MathTypes.h"

#include <format>
#include <glm/trigonometric.hpp>
String Size2D::ToString() {
    return std::format(L"Width: {}, Height:{}", Width, Height);
}

FVector3 FRotator::GetForwardVector() const {
    FVector3 Forward;
    Forward.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
    Forward.y = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
    Forward.z = sin(glm::radians(Pitch));
    return Forward;
}
