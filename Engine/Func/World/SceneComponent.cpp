//
// Created by Echo on 2025/3/25.
//

#include "SceneComponent.hpp"

#include "Actor.hpp"

Vector3f SceneComponent::GetWorldLocation() const
{
    return mWorldTransform.GetLocation();
}

void SceneComponent::SetLocation(const Vector3f& InLocation)
{
    if (InLocation == mTransform.GetLocation()) return;
    mTransform.SetLocation(InLocation);
    SetTransformDirty();
}

void SceneComponent::SetRotation(const Quaternionf& rotation)
{
    mTransform.SetRotation(rotation);
    SetTransformDirty();
}

void SceneComponent::SetRotator(const Rotatorf& InRotator)
{
    mTransform.SetRotation(InRotator);
    SetTransformDirty();
}

void SceneComponent::SetScale(const Vector3f& InScale)
{
    if (InScale == mTransform.GetScale())
        return;
    mTransform.SetScale(InScale);
    SetTransformDirty();
}

void SceneComponent::UpdateTransform(const Transform& InParentTransform)
{
    SetTransformDirty(false);
    mWorldTransform.Translate(InParentTransform.GetLocation());
    mWorldTransform.Scale(InParentTransform.GetScale());
    // TODO: 处理旋转
}
