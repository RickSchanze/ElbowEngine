//
// Created by Echo on 25-1-2.
//

#pragma once
#include "Core/Object/Object.h"
#include "Core/Object/ObjectPtr.h"
#include "Transform.h"

#include GEN_HEADER("Func.Actor.generated.h")


namespace func
{
class Component;
}

namespace func
{
class CLASS() Actor : public core::Object
{
    GENERATED_CLASS(Actor)
public:
    Transform&                     GetTransform() { return transform_; }
    void                           SetTransform(const Transform& transform);
    [[nodiscard]] core::Vector3    GetLocation() const { return transform_.location; }
    [[nodiscard]] core::Vector3    GetWorldLocation() const;
    void                           SetLocation(const core::Vector3& location) { transform_.location = location; }
    [[nodiscard]] core::Quaternion GetRotationQuaterion() const { return transform_.rotation; }
    void                           SetRotation(const core::Quaternion& rotation) { transform_.rotation = rotation; }

private:
    PROPERTY()
    Transform transform_;

    PROPERTY()
    core::Array<core::ObjectPtr<Component>> components_;
};
}   // namespace func
