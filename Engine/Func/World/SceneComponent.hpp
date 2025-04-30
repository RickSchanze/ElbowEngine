//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Component.hpp"
#include "Core/Core.hpp"
#include "Transform.hpp"

#include GEN_HEADER("SceneComponent.generated.hpp")

class ECLASS() SceneComponent : public Component
{
    GENERATED_BODY(SceneComponent)

public:
    const Transform& GetTransform() const
    {
        return mTransform;
    }

    const Transform& GetWorldTransform() const
    {
        return mWorldTransform;
    }

    Vector3f GetLocation() const
    {
        return mTransform.GetLocation();
    }

    Vector3f GetWorldLocation() const;

    void SetLocation(const Vector3f& InLocation);

    ELBOW_FORCE_INLINE Quaternionf GetWorldRotation() const
    {
        return mWorldTransform.GetRotation();
    }

    ELBOW_FORCE_INLINE Rotatorf GetWorldRotator() const
    {
        return mWorldTransform.GetRotator();
    }

    ELBOW_FORCE_INLINE Quaternionf GetRotation() const
    {
        return mTransform.GetRotation();
    }

    ELBOW_FORCE_INLINE Rotatorf GetRotator() const
    {
        return mTransform.GetRotator();
    }

    void SetRotation(const Quaternionf& rotation);

    void SetRotator(const Rotatorf& InRotator);

    Vector3f GetScale() const
    {
        return mWorldTransform.GetScale();
    }

    void SetScale(const Vector3f& InScale);

    virtual void UpdateTransform(const Transform& InParentTransform);

    void SetTransformDirty(bool dirty = true)
    {
        transform_dirty_ = dirty;
    }

    bool IsTransformDirty() const
    {
        return transform_dirty_;
    }

protected:
    EFIELD()
    Transform mTransform;
    Transform mWorldTransform;

    bool transform_dirty_ = true;
};

REGISTER_TYPE(SceneComponent)