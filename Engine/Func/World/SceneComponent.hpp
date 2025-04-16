//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Component.hpp"
#include "Core/Core.hpp"
#include "Transform.hpp"

class SceneComponent : public Component
{
    REFLECTED_CLASS(SceneComponent)

public:
    const Transform &GetTransform() const
    {
        return transform_;
    }

    const Transform &GetWorldTransform() const
    {
        return mWorldTransform;
    }

    void SetTransform(const Transform &transform);

    Vector3f GetLocation() const
    {
        return transform_.Location;
    }

    Vector3f GetWorldLocation() const;

    void SetLocation(const Vector3f &location);

    Quaternionf GetWorldRotationQuaterion() const;

    Quaternionf GetRotationQuaterion() const;

    Vector3f GetWorldRotation() const;

    Vector3f GetRotation() const;

    void SetRotation(const Quaternionf &rotation);

    void SetRotation(const Vector3f &eluer);

    Vector3f GetScale() const
    {
        return mWorldTransform.Scale;
    }

    void SetScale(const Vector3f &scale);

    virtual void UpdateTransform(const Transform &parent_transform);

    void SetTransformDirty(bool dirty = true)
    {
        transform_dirty_ = dirty;
    }

    bool IsTransformDirty() const
    {
        return transform_dirty_;
    }

    void Rotate(Vector3f eluer_degree);

protected:
    REFLECTED()
    Transform transform_;
    Transform mWorldTransform;

    bool transform_dirty_ = true;
};

REGISTER_TYPE(SceneComponent)