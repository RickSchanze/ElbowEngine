//
// Created by Echo on 2025/3/25.
//

#include "Actor.hpp"

IMPL_REFLECTED(Actor) {
    return Type::Create<Actor>("Actor") | refl_helper::AddParents<Object>() | refl_helper::AddField("transform", &Actor::transform_);
}

void Actor::SetTransform(const Transform &transform) { transform_ = transform; }

Vector3f Actor::GetWorldLocation() const { return GetLocation(); }
