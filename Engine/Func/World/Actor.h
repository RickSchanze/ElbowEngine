//
// Created by Echo on 25-1-2.
//

#pragma once
#include "Core/Object/Object.h"
#include "Transform.h"

#include GEN_HEADER("Func.Actor.generated.h")


namespace func
{
class CLASS() Actor : public core::Object
{
    GENERATED_CLASS(Actor)
public:
    Transform&       GetTransform() { return transform_; }
    void             SetTransform(const Transform& transform);
    core::Vector3    GetLocation() const { return transform_.location; }
    void             SetLocation(const core::Vector3& location) { transform_.location = location; }
    core::Quaternion GetRotationQuaterion() const { return transform_.rotation; }
    void             SetRotation(const core::Quaternion& rotation) { transform_.rotation = rotation; }

private:
    Transform transform_;
};
}   // namespace func
