/**
 * @file MathTypes.h
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#pragma once

#include "CoreDef.h"

#include <glm/vec3.hpp>

struct Size2D
{
    int32 Width;
    int32 Height;

    String ToString();
};

typedef glm::vec3 Vector3;
typedef glm::vec3 Vector4;
typedef glm::vec3 Color;
