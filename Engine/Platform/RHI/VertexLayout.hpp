//
// Created by Echo on 2025/3/22.
//

#pragma once

#include "Core/Math/Types.hpp"

namespace rhi {
    struct Vertex1 {
        Vector3f position;
        Vector3f normal;
        Vector2f texcoord;
    };

    struct InstancedData1 {
        Vector3f location;
        Vector3f rotation;
        Vector3f scale;
    };

    struct Vertex_UI {
        Vector3f position;
        Vector2f uv;
        Color color;
    };
} // namespace rhi
