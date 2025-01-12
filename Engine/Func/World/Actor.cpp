//
// Created by Echo on 25-1-2.
//

#include "Actor.h"
#include "Core/Core.h"

#include GEN_HEADER("Func.Actor.generated.h")

GENERATED_SOURCE()

using namespace func;
using namespace core;

void Actor::SetTransform(const Transform& transform)
{
    transform_ = transform;
}

Vector3 Actor::GetWorldLocation() const
{
    return GetLocation();
}