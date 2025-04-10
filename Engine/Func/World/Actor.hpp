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
    Vector3f GetLocation() const { return transform_.location; }
    Vector3f GetWorldLocation() const;
    void SetLocation(const Vector3f &location);
    Quaternionf GetRotationQuaterion() const { return transform_.GetRotationQuaterion(); }

    void SetRotation(const Quaternionf &rotation);
    void SetRotation(const Vector3f &eluer);

    Vector3f GetScale() const { return transform_.scale; }
    void SetScale(const Vector3f &scale);

    void PreTick(MilliSeconds delta_time) override;

    template<typename T>
    T *AddComponent() {
        T *comp = NewObject<T>();
        comp->SetOwner(this);
        components_.Add(comp);
        return comp;
    }

    bool IsTransformDirty() const { return transform_dirty_; }

protected:
    void UpdateTransform();
    void SetTransformDirty(bool dirty = true) { transform_dirty_ = dirty; }

private:
    REFLECTED()
    Transform transform_;

    bool transform_dirty_ = true;

    Array<ObjectPtr<Component>> components_;
};

REGISTER_TYPE(Actor)
