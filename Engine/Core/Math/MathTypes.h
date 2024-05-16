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

struct FRotator
{
    float Pitch;
    float Yaw;
    float Roll;

    FVector3 GetForwardVector() const;
};
