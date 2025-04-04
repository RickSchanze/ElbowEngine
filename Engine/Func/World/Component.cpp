//
// Created by Echo on 2025/3/25.
//

#include "Component.hpp"
#include "Actor.hpp"

IMPL_REFLECTED_INPLACE(Component) {
    return Type::Create<Component>("Component") | refl_helper::AddParents<Object>() | refl_helper::AddField("owner", &Component::owner_);
}

Component::Component() {
    name_ = "Component";
    SetDisplayName("空组件");
    flags_ |= OFT_Component;
}

void Component::SetOwner(const Actor *owner) { owner_ = owner; }
