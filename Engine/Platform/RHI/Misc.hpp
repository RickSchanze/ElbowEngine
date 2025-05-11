//
// Created by Echo on 2025/3/22.
//

#pragma once

#include "Core/Math/Vector.hpp"
#include "Core/Math/Color.hpp"

namespace NRHI {
    struct Vertex1 {
        Vector3f Position;
        Vector3f Normal;
        Vector2f UV0;
        Vector3f Tangent;
    };

    struct InstancedData1 {
        Vector3f Location;
        Vector3f Rotation;
        Vector3f Scale;
    };

    struct Vertex_UI {
        Vector3f position;
        Vector2f uv;
        Color color;
    };

    struct PointLight {
        Vector3f Location;
        UInt8 _Padding;
        Vector3f Color;
    };
} // namespace rhi
