/**
 * @file GameObject.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "Math/MathTypes.h"
#include "CoreGlobal.h"
#include "FunctionCommon.h"
#include "Object/Object.h"

namespace Function {
class Component;
}
FUNCTION_NAMESPACE_BEGIN

class GameObject : public Object {
public:
     GameObject(GameObject* InParent = nullptr);
    ~GameObject() override;

    void BeginPlay();
    void Tick(float DeltaTime) const;
    void EndPlay();

    template<typename T>
        requires std::derived_from<T, Component>
    T* AddComponent() {
        T* Component           = New<T>(STRING_NONE, this);
        Component->mGameObject = this;
        Component->BeginPlay();
        Component->transform_ = &mTransform;
        mComponents.push_back(Component);
        return Component;
    }

    template<typename T>
        requires std::derived_from<T, Component>
    T* GetComponent() const {
        for (auto component: mComponents) {
            if (dynamic_cast<T*>(component) != nullptr) {
                return dynamic_cast<T*>(component);
            }
        }
        return nullptr;
    }

    // 销毁一个Component
    void DestroyComponent(Component* InComponent);

    // 获取此对象的parent
    GameObject* GetParent() const { return mParentOject; }

    // 获取这个对象的所有子对象
    TArray<GameObject*>& GetChildren() { return mSubGameObjects; }

    // 这个对象是否有子对象？
    bool HasChildren() const { return !mSubGameObjects.empty(); }

    // 获取这个对象的变换 注意：返回非常量引用
    Transform& GetTransform() { return mTransform; }

    // 获取这个对象的所有Components
    TArray<Component*>& GetComponents() { return mComponents; }

public:
    static const TArray<GameObject*>& GetRootGameObjects() { return sRootObjects; }

protected:
    Transform           mTransform = {};
    TArray<Component*>  mComponents;
    TArray<GameObject*> mSubGameObjects;
    GameObject*         mParentOject = nullptr;

    static inline TArray<GameObject*> sRootObjects;
};

FUNCTION_NAMESPACE_END
