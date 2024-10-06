/**
 * @file GameObject.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "GameObject.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "World/TickManager.h"

using namespace function::comp;

namespace function
{

GameObject::GameObject(GameObject* parent) : Object(EOF_IsGameObject), transform_(this)
{
    if (parent == nullptr)
    {
        s_root_objects_.push_back(this);
    }
    else
    {
        parent_object_ = parent;
        parent->sub_game_objects_.push_back(this);
    }
    // 向TickManager加入自己
    TickManager::Get()->Add(this);
}

GameObject::~GameObject()
{
    TickManager::Get()->Remove(this);
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
    if (parent_object_ != nullptr)
    {
        std::erase(parent_object_->sub_game_objects_, this);
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

void GameObject::EndPlay()
{
    for (const auto Component: components_)
    {
        Component->EndPlay();
    }
}


void GameObject::PreTick() {}

void GameObject::Tick() {}

void GameObject::PostTick()
{
    if (transform_dirty_)
    {
        ApplyTransformDeltas();
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
    if (parent_object_ != nullptr)
    {
        parent_ = parent_object_->transform_;
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

}
