//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Math/Types.hpp"

struct Transform {
  REFLECTED_STRUCT(Transform)

  REFLECTED(Label = "位置")
  Vector3f location = {0, 0, 0};

  REFLECTED(Label = "缩放")
  Vector3f scale = {1, 1, 1};

  // TODO: UI上的Rotation不应该用四元数
  REFLECTED(Label = "旋转")
  Quaternionf rotation = {0, 0, 0, 0};
};

REGISTER_TYPE(Transform)
