/**
 * @file GameObject.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "GameObject.h"
#include "Component/Component.h"
#include "Component/Transform.h"

using namespace Function::Comp;

FUNCTION_NAMESPACE_BEGIN

GameObject::GameObject(GameObject* InParent) : Object(EOF_IsGameObject), transform_(this)
{
    if (InParent == nullptr)
    {
        s_root_objects_.push_back(this);
    }
    else
    {
        parent_oject_ = InParent;
        InParent->sub_game_objects_.push_back(this);
    }
}

GameObject::~GameObject()
{
    for (auto component: components_)
    {
        component->EndPlay();
        Delete(component);
    }
    components_.clear();
    for (int i = sub_game_objects_.size() - 1; i >= 0; i--)
    {
        Delete(sub_game_objects_[i]);
    }
    if (parent_oject_ != nullptr)
    {
        std::erase(parent_oject_->sub_game_objects_, this);
    }
    else
    {
        std::erase(s_root_objects_, this);
    }
}

void GameObject::BeginPlay()
{
    // 对于已经注册的进行BeginPlay
    for (auto* component: components_)
    {
        component->BeginPlay();
    }
}

void GameObject::PreTickComponents(float delta_time)
{
    for (auto* component: components_)
    {
        component->PreTick(delta_time);
    }
}

void GameObject::TickComponents(float delta_time)
{
    for (auto* component: components_)
    {
        component->Tick(delta_time);
    }
}

void GameObject::PostTickComponents(float delta_time)
{
    for (auto* comp: components_)
    {
        comp->PostTick(delta_time);
    }
}

void GameObject::EndPlay()
{
    for (const auto Component: components_)
    {
        Component->EndPlay();
    }
}
void GameObject::PreTickObject(float delta_time) {}

void GameObject::TickObject(float delta_time) {}

void GameObject::PostTickObject(float delta_time)
{
    if (transform_dirty_)
    {
        ApplyTransformDeltas();
    }
}

void GameObject::PreTick(float delta_time)
{
    PreTickComponents(delta_time);
    PreTickObject(delta_time);
}

void GameObject::Tick(float delta_time)
{
    TickComponents(delta_time);
    TickObject(delta_time);
}

void GameObject::PostTick(float delta_time)
{
    PostTickComponents(delta_time);
    PostTickObject(delta_time);
}

void GameObject::TickObjects(float delta_time)
{
    const auto& objs = ObjectManager::Get()->GetAllObject();
    for (auto* obj: objs | std::views::values)
    {
        if (obj->IsGameObject())
        {
            static_cast<GameObject*>(obj)->PreTick(delta_time);
        }
    }
    for (auto* obj: objs | std::views::values)
    {
        if (obj->IsGameObject())
        {
            static_cast<GameObject*>(obj)->Tick(delta_time);
        }
    }
    for (auto* obj: objs | std::views::values)
    {
        if (obj->IsGameObject())
        {
            static_cast<GameObject*>(obj)->PostTick(delta_time);
        }
    }
}

void GameObject::DestroyComponent(Component* component)
{
    component->EndPlay();
    std::erase(components_, component);
    Delete(component);
}

void GameObject::MarkTransformDirty()
{
    transform_dirty_ = true;
}

void GameObject::ForceUpdateTransform()
{
    ApplyTransformDeltas();
}

void GameObject::ApplyTransformDeltas()
{
    Transform parent_;
    if (parent_oject_ != nullptr)
    {
        parent_ = parent_oject_->transform_;
    }
    else
    {
        parent_ = Transform::Identity();
    }
    transform_.ApplyModify(parent_.world_position_, parent_.world_rotator_, parent_.world_scale_);
    for (auto* object: sub_game_objects_)
    {
        object->ApplyTransformDeltas();
    }
    transform_dirty_ = false;
}

FUNCTION_NAMESPACE_END
