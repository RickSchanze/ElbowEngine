//
// Created by Echo on 2025/3/25.
//

#include "Actor.hpp"

#include "Component.hpp"
#include "SceneComponent.hpp"

void Actor::SetTransform(const Transform &transform) { mTransform = transform; }

Vector3f Actor::GetWorldLocation() const { return GetLocation(); }

void Actor::SetLocation(const Vector3f &location) {
    mTransform.Location = location;
    SetTransformDirty();
}

void Actor::SetRotation(const Quaternionf &rotation) {
    mTransform.SetRotation(rotation);
    SetTransformDirty();
}

void Actor::SetRotation(const Vector3f &eluer) {
    mTransform.SetRotation(eluer);
    SetTransformDirty();
}

void Actor::SetScale(const Vector3f &scale) {
    mTransform.Scale = scale;
    SetTransformDirty();
}

void Actor::PreTick(MilliSeconds delta_time) { UpdateTransform(); }

void Actor::UpdateTransform() {
    bool actor_transform_dirty = IsTransformDirty();
    SetTransformDirty(false);
    for (Component *comp: components_) {
        if (comp->GetType()->IsDerivedFrom(SceneComponent::GetStaticType())) {
            SceneComponent *scene_comp = static_cast<SceneComponent *>(comp);
            if (scene_comp->IsTransformDirty() || actor_transform_dirty)
                scene_comp->UpdateTransform(mTransform);
        }
    }
}
