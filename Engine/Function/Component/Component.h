/**
 * @file Component.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "Object/Object.h"

#include "Component.generated.h"

struct Transform;

namespace Function {
class GameObject;
}

FUNCTION_COMPONENT_NAMESPACE_BAGIN

class REFL Component : public Object {
    GENERATED_BODY(Component)
public:
    friend class GameObject;

    Component();

    virtual void Tick(float DeltaTime) {}
    virtual void BeginPlay();
    virtual void EndPlay();
    virtual void OnEnable() {}
    virtual void OnDisable() {}

    void SetEnabled(bool enable);

    Transform& GetTransform() const;

    void Destroy();

protected:
    Transform* transform_ = nullptr;
    GameObject* game_object_ = nullptr;
    bool enabled_ = true;
};

FUNCTION_NAMESPACE_END
