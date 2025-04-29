//
// Created by Echo on 2025/3/25.
//

#include "SceneComponent.hpp"

#include "Actor.hpp"

void SceneComponent::SetTransform(const Transform &transform) { transform_ = transform; }

Vector3f SceneComponent::GetWorldLocation() const { return mWorldTransform.mLocation; }

void SceneComponent::SetLocation(const Vector3f &location) {
    transform_.mLocation = location;
    SetTransformDirty();
}

Quaternionf SceneComponent::GetWorldRotationQuaterion() const { return mWorldTransform.GetRotationQuaterion(); }

Quaternionf SceneComponent::GetRotationQuaterion() const { return transform_.GetRotationQuaterion(); }

Vector3f SceneComponent::GetWorldRotation() const { return mWorldTransform.GetRotationEuler(); }

Vector3f SceneComponent::GetRotation() const { return transform_.GetRotationEuler(); }

void SceneComponent::SetRotation(const Quaternionf &rotation) {
    transform_.SetRotation(rotation);
    SetTransformDirty();
}

void SceneComponent::SetRotation(const Vector3f &eluer) {
    transform_.SetRotation(eluer);
    SetTransformDirty();
}

void SceneComponent::SetScale(const Vector3f &scale) {
    transform_.mScale = scale;
    SetTransformDirty();
}

void SceneComponent::Rotate(Vector3f eluer_degree) {
    transform_.Rotate(eluer_degree);
    SetTransformDirty();
}

void SceneComponent::UpdateTransform(const Transform &parent_transform) {
    SetTransformDirty(false);
    mWorldTransform.mLocation = transform_.mLocation + parent_transform.mLocation;
    mWorldTransform.mScale = transform_.mScale * parent_transform.mScale;
    glm::quat parent_q = parent_transform.GetRotationQuaterion();
    glm::quat now_q = transform_.GetRotationQuaterion();
    mWorldTransform.SetRotation(parent_q * now_q);
}
