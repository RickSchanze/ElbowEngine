//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Math/Quaternion.hpp"
#include "Core/Math/Vector.hpp"

#include "Core/Math/Rotator.hpp"
#include GEN_HEADER("Transform.generated.hpp")

struct ESTRUCT() Transform
{
    GENERATED_BODY(Transform)

    void SetRotation(Quaternionf now);
    void SetRotator(Rotatorf now);

    ELBOW_FORCE_INLINE Quaternionf GetRotation() const
    {
        return mRotation;
    }

    ELBOW_FORCE_INLINE Rotatorf GetRotator() const
    {
        return mCachedEuler;
    }

    ELBOW_FORCE_INLINE Vector3f GetLocation() const
    {
        return mLocation;
    }

    ELBOW_FORCE_INLINE Vector3f GetScale() const
    {
        return mScale;
    }

    ELBOW_FORCE_INLINE void SetScale(const Vector3f Scale)
    {
        mScale = Scale;
    }

    ELBOW_FORCE_INLINE void SetLocation(const Vector3f InLocation)
    {
        mLocation = InLocation;
    }

    ELBOW_FORCE_INLINE void Translate(const Vector3f InDelta)
    {
        mLocation += InDelta;
    }

    ELBOW_FORCE_INLINE void Scale(const Vector3f Factor)
    {
        mScale *= Factor;
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
    Rotatorf mCachedEuler = {0, 0, 0};

    EFIELD()
    Quaternionf mRotation = {0, 0, 0, 1};
};
