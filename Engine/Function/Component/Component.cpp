/**
 * @file Component.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "Component.h"

#include "CoreGlobal.h"
#include "GameObject/GameObject.h"
#include "Utils/StringUtils.h"

#include "Component.generated.h"
#include "World/TickManager.h"

GENERATED_SOURCE()

namespace function::comp
{

Component::Component() : Object(EOF_IsComponent) {}

Component::~Component() = default;

void Component::BeginPlay()
{
    OnEnable();
}

void Component::EndPlay()
{
    OnDisable();
}

void Component::SetEnabled(const bool enable)
{
    if (enabled_ != enable)
    {
        enabled_ = enable;
        if (enabled_)
        {
            OnEnable();
        }
        else
        {
            OnDisable();
        }
    }
}

::Transform& Component::GetTransform() const
{
    return *transform_;
}

void Component::Destroy()
{
    game_object_->DestroyComponent(this);
}

Vector3 Component::GetPosition() const
{
    return transform_->GetPosition();
}

Vector3 Component::GetWorldPosition() const
{
    return transform_->GetWorldPosition();
}

TickableComponent::TickableComponent()
{
    TickManager::Get()->Add(this);
}

TickableComponent::~TickableComponent()
{
    TickManager::Get()->Remove(this);
}

}   // namespace function::comp
