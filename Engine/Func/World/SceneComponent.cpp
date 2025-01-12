//
// Created by Echo on 25-1-12.
//

#include "SceneComponent.h"
#include "Core/Reflection/CtorManager.h"
#include "Core/Reflection/Reflection.h"

#include "Actor.h"
#include "Core/Math/Math.h"
#include GEN_HEADER("Func.SceneComponent.generated.h")
#include "Func/Logcat.h"

using namespace func;
using namespace core;

GENERATED_SOURCE()

void SceneComponent::SetTransform(const Transform& transform)
{
    transform_ = transform;
}

Vector3 SceneComponent::GetWorldLocation() const
{
    const Actor* owner = static_cast<Actor*>(owner_);
    if (owner_)
    {
        return Math::Translate(owner->GetWorldLocation(), GetLocation());
    }
    LOGGER.Error(logcat::Func_Comp, "场景组件丢失Actor");
    return transform_.location;
}

void SceneComponent::SetLocation(const core::Vector3& location)
{
    transform_.location = location;
    SetDirty();
}

Quaternion SceneComponent::GetWorldRotationQuaterion() const
{
    const Actor* owner = static_cast<Actor*>(owner_);
    if (owner_)
    {
        return owner->GetRotationQuaterion();
    }
    LOGGER.Error(logcat::Func_Comp, "场景组件丢失Actor");
    return transform_.location;
}

Vector3 SceneComponent::GetWorldRotation() const
{
    return Math::ToEuler(GetWorldRotationQuaterion());
}

Vector3 SceneComponent::GetRotation() const
{
    return Math::ToEuler(GetRotationQuaterion());
}

void SceneComponent::SetRotation(const core::Quaternion& rotation)
{
    transform_.rotation = rotation;
    SetDirty();
}
