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
    Transform &GetTransform() { return transform_; }
    void SetTransform(const Transform &transform);

    [[nodiscard]] Vector3f GetLocation() const { return transform_.location; }
    [[nodiscard]] Vector3f GetWorldLocation() const;
    void SetLocation(const Vector3f &location);

    [[nodiscard]] Quaternionf GetWorldRotationQuaterion() const;
    [[nodiscard]] Quaternionf GetRotationQuaterion() const { return transform_.rotation; }
    [[nodiscard]] Vector3f GetWorldRotation() const;
    [[nodiscard]] Vector3f GetRotation() const;
    void SetRotation(const Quaternionf &rotation);

protected:
    REFLECTED()
    Transform transform_;
};

REGISTER_TYPE(SceneComponent)
