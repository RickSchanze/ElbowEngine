/**
 * @file TransformComponent.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "Component.h"
#include "FunctionCommon.h"
#include "Math/MathTypes.h"
#include "Transform.generated.h"

FUNCTION_NAMESPACE_BEGIN

class REFL Transform  {
    GENERATED_BODY(Transform)

public:
    PROPERTY(Serialized)
    FVector3 Position = {0, 0, 0};

    PROPERTY(Serialized)
    FRotator Rotation = {0, 0, 0};

    PROPERTY(Serialized)
    FVector3 Scale    = {1, 1, 1};

    FVector3 GetForwardVector() const;
    FVector3 GetUpVector() const;
    FVector3 GetRightVector() const;
};

FUNCTION_NAMESPACE_END
