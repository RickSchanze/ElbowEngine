//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Object/Object.hpp"
#include "Transform.hpp"


class Actor : public Object {
    REFLECTED_CLASS(Actor)
public:
    Transform &GetTransform() { return transform_; }
    void SetTransform(const Transform &transform);
    [[nodiscard]] Vector3f GetLocation() const { return transform_.location; }
    [[nodiscard]] Vector3f GetWorldLocation() const;
    void SetLocation(const Vector3f &location) { transform_.location = location; }
    [[nodiscard]] Quaternionf GetRotationQuaterion() const { return transform_.rotation; }
    void SetRotation(const Quaternionf &rotation) { transform_.rotation = rotation; }

protected:
    template<typename T>
    T *CreateComponent() {
        T *comp = NewObject<T>();
        comp->SetOwner(this);
        return comp;
    }

private:
    REFLECTED()
    Transform transform_;
};

REGISTER_TYPE(Actor)
