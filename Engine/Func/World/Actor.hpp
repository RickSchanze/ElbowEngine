//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Object/Object.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "ITick.hpp"
#include "Transform.hpp"

#include GEN_HEADER("Actor.generated.hpp")

class Component;
class ECLASS() Actor : public Object, public ITick {
    GENERATED_BODY(Actor)
public:
    Transform &GetTransform() { return mTransform; }
    void SetTransform(const Transform &transform);
    Vector3f GetLocation() const { return mTransform.GetLocation(); }
    Vector3f GetWorldLocation() const;
    void SetLocation(const Vector3f &location);
    Quaternionf GetRotationQuaterion() const { return mTransform.GetRotationQuaterion(); }

    void SetRotation(const Quaternionf &rotation);
    void SetRotation(const Vector3f &eluer);

    Vector3f GetScale() const { return mTransform.GetScale(); }
    void SetScale(const Vector3f& scale);

    virtual void PreTick(MilliSeconds delta_time) override;

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
    EFIELD()
    Transform mTransform;

    bool transform_dirty_ = true;

    Array<ObjectPtr<Component>> components_;
};
