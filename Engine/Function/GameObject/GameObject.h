/**
 * @file GameObject.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "CoreGlobal.h"
#include "FunctionCommon.h"
#include "Math/MathTypes.h"
#include "Object/Object.h"

namespace Function::Comp
{
class Component;
}

FUNCTION_NAMESPACE_BEGIN

class GameObject : public Object
{
public:
    explicit GameObject(GameObject* InParent = nullptr);

    ~GameObject() override;

    void BeginPlay();
    void Tick(float delta_time) const;
    void EndPlay();

    template<typename T>
        requires std::derived_from<T, Comp::Component>
    T* AddComponent()
    {
        T* component           = New<T>();
        component->game_object_ = this;
        component->BeginPlay();
        component->transform_ = &transform_;
        components_.push_back(component);
        return component;
    }

    template<typename T>
        requires std::derived_from<T, Comp::Component>
    T* GetComponent() const
    {
        for (auto component: components_)
        {
            if (dynamic_cast<T*>(component) != nullptr)
            {
                return dynamic_cast<T*>(component);
            }
        }
        return nullptr;
    }

    // 销毁一个Component
    void DestroyComponent(Comp::Component* component);

    // 获取此对象的parent
    GameObject* GetParent() const { return parent_oject_; }

    // 获取这个对象的所有子对象
    TArray<GameObject*>& GetChildren() { return sub_game_objects_; }

    // 这个对象是否有子对象？
    bool HasChildren() const { return !sub_game_objects_.empty(); }

    // 获取这个对象的变换 注意：返回非常量引用
    Transform& GetTransform() { return transform_; }

    // 获取这个对象的所有Components
    TArray<Comp::Component*>& GetComponents() { return components_; }

public:
    static const TArray<GameObject*>& GetRootGameObjects() { return s_root_objects_; }

protected:
    Transform           transform_ = {};
    TArray<Comp::Component*>  components_;
    TArray<GameObject*> sub_game_objects_;
    GameObject*         parent_oject_ = nullptr;

    static inline TArray<GameObject*> s_root_objects_;
};

FUNCTION_NAMESPACE_END
