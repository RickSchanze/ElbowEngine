//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Math/Types.hpp"

#include GEN_HEADER("Transform.generated.hpp")

struct ESTRUCT() Transform {
    GENERATED_BODY(Transform)

    EFIELD()
    Vector3f Location = {0, 0, 0};

    EFIELD()
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
    EFIELD()
    Quaternionf mRotation = {0, 0, 0, 1};

    EFIELD(Transient)
    Vector3f mCachedEuler = {0, 0, 0};
};
