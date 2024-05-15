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

class TransformComponent final : public ComponentBase {
public:
    TransformComponent() : ComponentBase(L"Transform", nullptr) {}

    Vector3 Position = {0, 0, 0};
    Vector3 Rotation = {0, 0, 0};
    Vector3 Scale    = {1, 1, 1};
};

FUNCTION_NAMESPACE_END
