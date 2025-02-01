//
// Created by Echo on 25-1-3.
//

#pragma once
#include "Core/Math/MathTypes.h"

namespace platform::rhi {

struct Vertex1 {
  core::Vector3 position;
  core::Vector3 normal;
  core::Vector2 texcoord;
};

struct InstancedData1 {
  core::Vector3 location;
  core::Vector3 rotation;
  core::Vector3 scale;
};

struct Vertex_UI {
  core::Vector3 position;
  core::Vector2 uv;
};

} // namespace platform::rhi
