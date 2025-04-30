//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Math/Quaternion.hpp"
#include "Core/Math/Vector.hpp"

#include GEN_HEADER("Transform.generated.hpp")

struct ESTRUCT() Transform
{
    GENERATED_BODY(Transform)

    void SetRotation(Quaternionf now);
    void SetRotation(Vector3f now);

    ELBOW_FORCE_INLINE Vector3f GetRotationEuler() const
    {
        return mCachedEuler;
    }

    ELBOW_FORCE_INLINE Quaternionf GetRotationQuaterion() const
    {
        return mRotation;
    }

    ELBOW_FORCE_INLINE Vector3f GetLocation() const
    {
        return mLocation;
    }

    ELBOW_FORCE_INLINE Vector3f GetScale() const
    {
        return mScale;
    }

    Vector3f GetForwardVector() const;
    Vector3f GetRightVector() const;

    void Rotate(Vector3f rot);

private:
    EFIELD()
    Vector3f mLocation = {0, 0, 0};

    EFIELD()
    Vector3f mScale = {1, 1, 1};

    EFIELD(Transient)
    Vector3f mCachedEuler = {0, 0, 0};

    EFIELD()
    Quaternionf mRotation = {0, 0, 0, 1};
};
