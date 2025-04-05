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
    return world_transform_.location;
}

void SceneComponent::SetLocation(const Vector3f &location) {
    transform_.location = location;
    SetTransformDirty();
}

Quaternionf SceneComponent::GetWorldRotationQuaterion() const {
    return world_transform_.GetRotationQuaterion();
}

Quaternionf SceneComponent::GetRotationQuaterion() const { return transform_.GetRotationQuaterion(); }

Vector3f SceneComponent::GetWorldRotation() const { return world_transform_.GetRotationEuler(); }

Vector3f SceneComponent::GetRotation() const { return transform_.GetRotationEuler(); }

void SceneComponent::SetRotation(const Quaternionf &rotation) {
    transform_.SetRotation(rotation);
    SetTransformDirty();
}

void SceneComponent::SetScale(const Vector3f &scale) {
    transform_.scale = scale;
    SetTransformDirty();
}

void SceneComponent::Rotate(Vector3f eluer_degree) {
    transform_.Rotate(eluer_degree);
    SetTransformDirty();
}

void SceneComponent::UpdateTransform(const Transform &parent_transform) {
    SetTransformDirty(false);
    world_transform_.location = transform_.location + parent_transform.location;
    world_transform_.scale = transform_.scale * parent_transform.scale;
    glm::quat parent_q = parent_transform.GetRotationQuaterion();
    glm::quat now_q = transform_.GetRotationQuaterion();
    world_transform_.SetRotation(parent_q * now_q);
}
