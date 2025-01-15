//
// Created by Echo on 25-1-12.
//

#include "Component.h"
#include "Actor.h"
#include "Core/Reflection/CtorManager.h"
#include "Core/Reflection/Reflection.h"
#include GEN_HEADER("Func.Component.generated.h")

using namespace func;
using namespace core;

GENERATED_SOURCE()

Component::Component()
{
    name_ = "Component";
    SetDisplayName("空组件");
    flags_ |= OFT_Component;
}

void Component::SetOwner(const Actor* owner)
{
    owner_ = owner;
}

void Component::SetDirty()
{
    dirty_ = true;
}