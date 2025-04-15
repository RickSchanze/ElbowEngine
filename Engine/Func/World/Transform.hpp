//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Math/Types.hpp"

struct Transform {
    REFLECTED_STRUCT(Transform)

    REFLECTED(Label = "位置")
    Vector3f Location = {0, 0, 0};

    REFLECTED(Label = "缩放")
    Vector3f Scale = {1, 1, 1};

    void SetRotation(Quaternionf now);
    void SetRotation(Vector3f now);
    Vector3f GetRotationEuler() const { return mCachedEuler; }
    Quaternionf GetRotationQuaterion() const { return mRotation; }
    Vector3f GetLocation() const { return Location; }
    Vector3f GetScale() const { return Scale; }
    Vector3f GetForwardVector() const;
    Vector3f GetRightVector() const;

    void Rotate(Vector3f rot);

private:
    // TODO: UI上的Rotation不应该用四元数
    REFLECTED(Label = "旋转")
    Quaternionf mRotation = {0, 0, 0, 1};

    Vector3f mCachedEuler = {0, 0, 0};
};

REGISTER_TYPE(Transform)
