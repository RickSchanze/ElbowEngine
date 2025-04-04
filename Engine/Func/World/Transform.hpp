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

    void SetRotation(Quaternionf now);
    Vector3f GetRotationEuler() const { return cached_euler_; }
    Quaternionf GetRotationQuaterion() const { return rotation; }
    Vector3f GetLocation() const { return location; }
    Vector3f GetScale() const { return scale; }

private:
    // TODO: UI上的Rotation不应该用四元数
    REFLECTED(Label = "旋转")
    Quaternionf rotation = {0, 0, 0, 1};

    Vector3f cached_euler_ = {0, 0, 0};
};

REGISTER_TYPE(Transform)
