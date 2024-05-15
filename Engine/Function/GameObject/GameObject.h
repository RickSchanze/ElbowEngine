/**
 * @file GameObject.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "Component/TransformComponent.h"
#include "FunctionCommon.h"
#include "Object/Object.h"

namespace Function {
class ComponentBase;
}
FUNCTION_NAMESPACE_BEGIN

class GameObject : public Object {
public:
    void BeginPlay();
    void Tick() const;
    void EndPlay();

    template<typename T>
        requires std::derived_from<T, ComponentBase>
    T* AddComponent(const String& InComponentName) {
        T* Component = new T(InComponentName, this);
        Component->mGameObject = this;
        Component->BeginPlay();
        Component->mTransform = &mTransform;
        mComponents.push_back(Component);
        return Component;
    }

    template<typename T>
        requires std::derived_from<T, ComponentBase>
    T* GetComponent() const {
        for (auto component: mComponents) {
            if (dynamic_cast<T*>(component) != nullptr) {
                return dynamic_cast<T*>(component);
            }
        }
        return nullptr;
    }

    void DestroyComponent(ComponentBase* InComponent);

protected:
    TransformComponent mTransform = {};
    TArray<ComponentBase*> mComponents;
};

FUNCTION_NAMESPACE_END
