/**
 * @file Component.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once

#include "Object/Object.h"
#include "Math/MathTypes.h"

#include "Component.generated.h"
#include "World/ITickable.h"

class Transform;

namespace function
{
class GameObject;
}

namespace function::comp {

ECLASS()
class Component : public Object
{
    GENERATED_BODY(Component)
public:
    friend class GameObject;

    Component();

    ~Component() override;

    virtual void BeginPlay();
    virtual void EndPlay();
    virtual void OnEnable() {}
    virtual void OnDisable() {}

    bool CanInstanced() const { return can_instanced; }

    void SetEnabled(bool enable);

    Transform& GetTransform() const;

    void Destroy();

    Vector3 GetPosition() const;
    Vector3 GetWorldPosition() const;

protected:
    Transform*  transform_   = nullptr;
    GameObject* game_object_ = nullptr;
    bool        enabled_     = true;

    // 能否被AddComponent函数实例化
    bool can_instanced = true;
};

ECLASS()
class TickableComponent : public Component, public ITickable
{
public:
    TickableComponent();
    ~TickableComponent() override;

    void PreTick() override {}
    void Tick() override {}
    void PostTick() override {}
};

}
