//
// Created by Echo on 25-1-12.
//

#pragma once

#include "Core/Object/Object.h"
#include "Core/Object/ObjectPtr.h"

#include GEN_HEADER("Func.Component.generated.h")

namespace func
{
class Actor;
class CLASS() Component : public core::Object
{
    GENERATED_CLASS(Component)
public:
    Component();
    Component(const Component&) = delete;

    void SetOwner(const Actor* owner);

    Actor* GetOwner() const { return owner_; }

    void SetDirty();

    [[nodiscard]] bool IsDirty() const { return dirty_; }

protected:
    PROPERTY()
    core::ObjectPtr<Actor> owner_ = nullptr;

    bool dirty_ = false;
};
}   // namespace func
