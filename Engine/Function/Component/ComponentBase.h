/**
 * @file ComponentBase.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "Object/Object.h"

namespace Function {
class GameObject;
class Transform;
}
FUNCTION_NAMESPACE_BEGIN

class Component : public Object {
public:
    friend class GameObject;
    explicit Component(String InName, GameObject* InGameObject);

    virtual void Tick(float DeltaTime) {}
    virtual void BeginPlay();
    virtual void EndPlay() {}

    Transform& GetTransform() const;

    void Destroy();

protected:
    Transform* mTransform = nullptr;
    GameObject* mGameObject = nullptr;
};

FUNCTION_NAMESPACE_END
