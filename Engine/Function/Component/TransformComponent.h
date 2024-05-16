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

class TransformComponent final : public Component {
public:
    TransformComponent() : Component(String(L"Transform"), nullptr) {}

    FVector3 Position = {0, 0, 0};
    FRotator Rotation = {0, 0, 0};
    FVector3 Scale    = {1, 1, 1};
};

FUNCTION_NAMESPACE_END
