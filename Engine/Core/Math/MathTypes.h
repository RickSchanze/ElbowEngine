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
    int32 Width;
    int32 Height;

    String ToString();
};


typedef glm::vec3 FVector3;
typedef glm::vec2 FVector2;
typedef glm::vec3 Color;

namespace Constant {
constexpr FVector3 ZeroVector    = {0, 0, 0};
constexpr FVector3 OneVector     = {1, 1, 1};
constexpr FVector3 ForwardVector = {0, 0, 1};
constexpr FVector3 RightVector   = {1, 0, 0};
constexpr FVector3 UpVector      = {0, 1, 0};
}   // namespace Constant

struct FRotator
{
    float Yaw = -90;
    float Roll = 0;
    float Pitch = 0;

    FVector3 GetForwardVector() const;
    FVector3 GetUpVector() const;
    FVector3 GetRightVector() const;
};


