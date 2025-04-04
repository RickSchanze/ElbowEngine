//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Object/Object.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "ITick.hpp"
#include "Transform.hpp"


class Component;
class Actor : public Object, public ITick {
    REFLECTED_CLASS(Actor)
public:
    Transform &GetTransform() { return transform_; }
    void SetTransform(const Transform &transform);
    [[nodiscard]] Vector3f GetLocation() const { return transform_.location; }
    [[nodiscard]] Vector3f GetWorldLocation() const;
    void SetLocation(const Vector3f &location) { transform_.location = location; }
    [[nodiscard]] Quaternionf GetRotationQuaterion() const { return transform_.GetRotationQuaterion(); }
    void SetRotation(const Quaternionf &rotation) { transform_.SetRotation(rotation); }

    void PreTick(MilliSeconds delta_time) override;

    template<typename T>
    T *AddComponent() {
        T *comp = NewObject<T>();
        comp->SetOwner(this);
        components_.Add(comp);
        return comp;
    }

protected:
    void UpdateTransform();

private:
    REFLECTED()
    Transform transform_;

    Array<ObjectPtr<Component>> components_;
};

REGISTER_TYPE(Actor)
