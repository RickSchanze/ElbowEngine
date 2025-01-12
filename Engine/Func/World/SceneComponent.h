//
// Created by Echo on 25-1-12.
//

#pragma once
#include "Component.h"

#include GEN_HEADER("Func.SceneComponent.generated.h")
#include "Transform.h"

namespace func
{
class CLASS() SceneComponent : public Component
{
    GENERATED_CLASS(SceneComponent)

public:
    Transform& GetTransform() { return transform_; }
    void       SetTransform(const Transform& transform);

    [[nodiscard]] core::Vector3 GetLocation() const { return transform_.location; }
    [[nodiscard]] core::Vector3 GetWorldLocation() const;
    void                        SetLocation(const core::Vector3& location);

    [[nodiscard]] core::Quaternion GetWorldRotationQuaterion() const;
    [[nodiscard]] core::Quaternion GetRotationQuaterion() const { return transform_.rotation; }
    [[nodiscard]] core::Vector3    GetWorldRotation() const;
    [[nodiscard]] core::Vector3    GetRotation() const;
    void                           SetRotation(const core::Quaternion& rotation);

protected:
    PROPERTY()
    Transform transform_;
};
}   // namespace func
