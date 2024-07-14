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

GENERATED_SOURCE()

FUNCTION_COMPONENT_NAMESPACE_BAGIN

Component::Component() : Object(EOF_IsComponent)
{
}

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

Transform& Component::GetTransform() const
{
    return *transform_;
}

void Component::Destroy()
{
    game_object_->DestroyComponent(this);
}

FUNCTION_COMPONENT_NAMESPACE_END
