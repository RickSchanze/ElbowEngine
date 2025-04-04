//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Component.hpp"
#include "Core/Core.hpp"
#include "Transform.hpp"

class SceneComponent : public Component {
    REFLECTED_CLASS(SceneComponent)

public:
    const Transform &GetTransform() const { return transform_; }
    const Transform &GetWorldTransform() const { return world_transform_; }
    void SetTransform(const Transform &transform);

    [[nodiscard]] Vector3f GetLocation() const { return transform_.location; }
    [[nodiscard]] Vector3f GetWorldLocation() const;
    void SetLocation(const Vector3f &location);

    [[nodiscard]] Quaternionf GetWorldRotationQuaterion() const;
    [[nodiscard]] Quaternionf GetRotationQuaterion() const;
    [[nodiscard]] Vector3f GetWorldRotation() const;
    [[nodiscard]] Vector3f GetRotation() const;
    void SetRotation(const Quaternionf &rotation);

    virtual void UpdateTransform(const Transform &parent_transform);

    void SetTransformDirty(bool dirty = true) { transform_dirty_ = dirty; }
    bool IsTransformDirty() const { return transform_dirty_; }

protected:
    REFLECTED()
    Transform transform_;
    Transform world_transform_;

    bool transform_dirty_ = true;
};

REGISTER_TYPE(SceneComponent)
