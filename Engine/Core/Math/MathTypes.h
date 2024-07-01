/**
 * @file MathTypes.h
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#pragma once

#include "CoreDef.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Size2D
{
    Int32 width;
    Int32 height;

    String ToString();
};


typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;
typedef glm::vec3 Color;

namespace Constant {
constexpr Vector3 ZeroVector    = {0, 0, 0};
constexpr Vector3 OneVector     = {1, 1, 1};
constexpr Vector3 ForwardVector = {0, 0, 1};
constexpr Vector3 RightVector   = {1, 0, 0};
constexpr Vector3 UpVector      = {0, 1, 0};
}   // namespace Constant

struct Rotator
{
    float yaw = -90;
    float roll = 0;
    float pitch = 0;

    Vector3 GetForwardVector() const;
    Vector3 GetUpVector() const;
    Vector3 GetRightVector() const;
};


