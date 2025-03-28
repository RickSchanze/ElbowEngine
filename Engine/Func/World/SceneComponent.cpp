//
// Created by Echo on 2025/3/25.
//

#include "SceneComponent.hpp"

#include "Actor.hpp"

IMPL_REFLECTED(SceneComponent) {
  return Type::Create<SceneComponent>("SceneComponent") | refl_helper::AddField("transform", &SceneComponent::transform_) |
         refl_helper::AddParents<Component>();
}

void SceneComponent::SetTransform(const Transform &transform) { transform_ = transform; }

Vector3f SceneComponent::GetWorldLocation() const {
  if (const Actor *owner = static_cast<Actor *>(owner_)) {
    return owner->GetWorldLocation() + GetLocation();
  }
  Log(Error) << "场景组件丢失Actor";
  return transform_.location;
}

void SceneComponent::SetLocation(const Vector3f &location) {
  transform_.location = location;
  SetDirty();
}

Quaternionf SceneComponent::GetWorldRotationQuaterion() const {
  if (const Actor *owner = static_cast<Actor *>(owner_)) {
    return owner->GetRotationQuaterion();
  }
  Log(Error) << "场景组件丢失Actor";
  return transform_.rotation;
}

Vector3f SceneComponent::GetWorldRotation() const { return GetWorldRotationQuaterion().ToEulerAngle(); }

Vector3f SceneComponent::GetRotation() const { return GetRotationQuaterion().ToEulerAngle(); }

void SceneComponent::SetRotation(const Quaternionf &rotation) {
  transform_.rotation = rotation;
  SetDirty();
}