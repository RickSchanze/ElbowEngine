//
// Created by Echo on 2025/3/25.
//

#include "SceneComponent.hpp"

#include "Actor.hpp"

void SceneComponent::SetTransform(const Transform &transform) { mTransform = transform; }

Vector3f SceneComponent::GetWorldLocation() const { return mWorldTransform.mLocation; }

void SceneComponent::SetLocation(const Vector3f &location) {
    mTransform.mLocation = location;
    SetTransformDirty();
}

Quaternionf SceneComponent::GetWorldRotationQuaterion() const { return mWorldTransform.GetRotationQuaterion(); }

Quaternionf SceneComponent::GetRotationQuaterion() const { return mTransform.GetRotationQuaterion(); }

Vector3f SceneComponent::GetWorldRotation() const { return mWorldTransform.GetRotationEuler(); }

Vector3f SceneComponent::GetRotation() const { return mTransform.GetRotationEuler(); }

void SceneComponent::SetRotation(const Quaternionf &rotation) {
    mTransform.SetRotation(rotation);
    SetTransformDirty();
}

void SceneComponent::SetRotation(const Vector3f &eluer) {
    mTransform.SetRotation(eluer);
    SetTransformDirty();
}

void SceneComponent::SetScale(const Vector3f &scale) {
    mTransform.mScale = scale;
    SetTransformDirty();
}

void SceneComponent::Rotate(Vector3f eluer_degree) {
    mTransform.Rotate(eluer_degree);
    SetTransformDirty();
}

void SceneComponent::UpdateTransform(const Transform &parent_transform) {
    SetTransformDirty(false);
    mWorldTransform.mLocation = mTransform.mLocation + parent_transform.mLocation;
    mWorldTransform.mScale = mTransform.mScale * parent_transform.mScale;
    glm::quat parent_q = parent_transform.GetRotationQuaterion();
    glm::quat now_q = mTransform.GetRotationQuaterion();
    mWorldTransform.SetRotation(parent_q * now_q);
}
