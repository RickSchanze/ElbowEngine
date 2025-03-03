//
// Created by Echo on 25-1-11.
//

#pragma once
#include "Core/Math/MathTypes.h"

#include GEN_HEADER("Func.Transform.generated.h")

namespace func {

struct STRUCT() Transform {
  GENERATED_STRUCT(Transform)

  PROPERTY(Label = "位置")
  core::Vector3 location = {0, 0, 0};

  PROPERTY(Label = "缩放")
  core::Vector3 scale = {1, 1, 1};

  // TODO: UI上的Rotation不应该用四元数
  PROPERTY(Label = "旋转")
  core::Quaternion rotation = {1, 0, 0, 0};
};
} // namespace func
