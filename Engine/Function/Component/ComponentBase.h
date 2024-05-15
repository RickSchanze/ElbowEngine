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
class TransformComponent;
}
FUNCTION_NAMESPACE_BEGIN

class ComponentBase : public Object {
public:
    friend class GameObject;
    explicit ComponentBase(String InName, GameObject* InGameObject);

    virtual void Tick() {}
    virtual void BeginPlay();
    virtual void EndPlay() {}

    void Destroy();

protected:
    TransformComponent* mTransform = nullptr;
    GameObject* mGameObject = nullptr;
};

FUNCTION_NAMESPACE_END
