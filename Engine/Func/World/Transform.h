//
// Created by Echo on 25-1-11.
//

#pragma once
#include "Core/Math/MathTypes.h"

#include GEN_HEADER("Func.Transform.generated.h")

namespace func
{

class CLASS() Transform
{
    GENERATED_CLASS(Transform)
public:
    PROPERTY(Label = "位置")
    core::Vector3 location = {};

    PROPERTY(Label = "缩放")
    core::Vector3 scale = {};

    // TODO: UI上的Rotation不应该用四元数
    PROPERTY(Label = "旋转")
    core::Quaternion rotation = {};
};
}   // namespace func
