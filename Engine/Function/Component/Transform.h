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
    Vector3 Position = {0, 0, 0};

    PROPERTY(Serialized)
    Rotator Rotation = {0, 0, 0};

    PROPERTY(Serialized)
    Vector3 Scale    = {1, 1, 1};

    Vector3 GetForwardVector() const;
    Vector3 GetUpVector() const;
    Vector3 GetRightVector() const;
};

FUNCTION_NAMESPACE_END
