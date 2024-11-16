/**
 * @file GameObject.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "Component/Transform.h"
#include "Core/CoreGlobal.h"
#include "Core/Object/Object.h"
#include "World/ITickable.h"

namespace function::comp
{
class Component;
}   // namespace Function::Comp

namespace function {

class GameObject : public Object, public ITickable
{
public:
    explicit GameObject(GameObject* parent = nullptr);

    ~GameObject() override;

    void BeginPlay();
    void EndPlay();

    void PreTick() override;
    void Tick() override;
    void PostTick() override;

    template<typename T>
        requires std::derived_from<T, comp::Component>
    T* AddComponent()
    {
        T* component = NewObject<T>();
        if (!component->can_instanced)
        {
            LOG_ERROR_CATEGORY(Object.Component, L"组件{}不能被AddComponent实例化", component->GetName());
            component->Destroy();
            return nullptr;
        }
        component->game_object_ = this;
        component->BeginPlay();
        component->transform_ = &transform_;
        components_.push_back(component);
        return component;
    }

    template<typename T>
        requires std::derived_from<T, comp::Component>
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
    void DestroyComponent(comp::Component* component);

    // 获取此对象的parent
    GameObject* GetParent() const { return parent_object_; }

    // 获取这个对象的所有子对象
    Array<GameObject*>& GetChildren() { return sub_game_objects_; }

    // 这个对象是否有子对象？
    bool HasChildren() const { return !sub_game_objects_.empty(); }

    // 获取这个对象的变换 注意：返回非常量引用
    Transform& GetTransform() { return transform_; }

    // 获取这个对象的所有Components
    Array<comp::Component*>& GetComponents() { return components_; }

    static const Array<GameObject*>& GetRootGameObjects() { return s_root_objects_; }

    void MarkTransformDirty();

    void ForceUpdateTransform();

private:
    void ApplyTransformDeltas();

protected:
    Transform transform_;
    bool      transform_dirty_{};   // transform有变化时为true

    Array<comp::Component*> components_;
    Array<GameObject*>      sub_game_objects_;
    GameObject*              parent_object_ = nullptr;

    static inline Array<GameObject*> s_root_objects_;
};

}
