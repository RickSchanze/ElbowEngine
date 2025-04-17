//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Object/Object.hpp"
#include "Core/Object/ObjectPtr.hpp"

#include GEN_HEADER("Component.generated.hpp")

class Actor;
class ECLASS() Component : public Object {
    GENERATED_BODY(Component)
public:
    Component();
    Component(const Component &) = delete;

    void SetOwner(const Actor *owner);

    Actor *GetOwner() const { return owner_; }

protected:
    EFIELD()
    ObjectPtr<Actor> owner_ = nullptr;
};
