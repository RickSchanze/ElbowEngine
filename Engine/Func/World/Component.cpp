//
// Created by Echo on 2025/3/25.
//

#include "Component.hpp"
#include "Actor.hpp"


Component::Component() {
    mName = "Component";
    SetDisplayName("空组件");
    mFlags |= OFT_Component;
}

void Component::SetOwner(const Actor *owner) { owner_ = owner; }
