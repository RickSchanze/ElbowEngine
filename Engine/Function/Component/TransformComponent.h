/**
 * @file TransformComponent.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "ComponentBase.h"
#include "FunctionCommon.h"
#include "Math/MathTypes.h"

FUNCTION_NAMESPACE_BEGIN

class Transform  {
public:
    FVector3 Position = {0, 0, 0};
    FRotator Rotation = {0, 0, 0};
    FVector3 Scale    = {1, 1, 1};

    FVector3 GetForwardVector() const;
    FVector3 GetUpVector() const;
    FVector3 GetRightVector() const;
};

FUNCTION_NAMESPACE_END
