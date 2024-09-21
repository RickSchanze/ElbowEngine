/**
 * @file GameObject.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "Component/Transform.h"
#include "CoreGlobal.h"
#include "FunctionCommon.h"
#include "Object/Object.h"

namespace Function::Comp
{
class Component;
}   // namespace Function::Comp

FUNCTION_NAMESPACE_BEGIN

class GameObject : public Object
{
public:
    explicit GameObject(GameObject* InParent = nullptr);

    ~GameObject() override;

    // TODO: 这里时序有点问题 GameObject不应该Tick 应该是Component Tick
    void BeginPlay();
    void EndPlay();

    // Tick所有Component
    void PreTickComponents(float delta_time);
    void TickComponents(float delta_time);
    void PostTickComponents(float delta_time);

    // GameObject本身做的Tick
    // 对transform的变化的实施会在PostTickObject函数里
    void PreTickObject(float delta_time);
    void TickObject(float delta_time);
    void PostTickObject(float delta_time);

    // 先TickComponent 再TickObject
    // PreTickComponents -> PreTickObject -> TickComponents -> TickObject -> PostTickComponents -> PostTickObject
    void PreTick(float delta_time);
    void Tick(float delta_time);
    void PostTick(float delta_time);

    /**
     * Tick所有GameObject
     * @param delta_time
     */
    static void TickObjects(float delta_time);

    template<typename T>
        requires std::derived_from<T, Comp::Component>
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

    static const TArray<GameObject*>& GetRootGameObjects() { return s_root_objects_; }

    void MarkTransformDirty();

    void ForceUpdateTransform();

private:
    void ApplyTransformDeltas();

protected:
    Transform transform_;
    bool      transform_dirty_;   // transform有变化时为true

    TArray<Comp::Component*> components_;
    TArray<GameObject*>      sub_game_objects_;
    GameObject*              parent_oject_ = nullptr;

    static inline TArray<GameObject*> s_root_objects_;
};

FUNCTION_NAMESPACE_END
