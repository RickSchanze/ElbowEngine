/**
 * @file GameObject.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "GameObject.h"
#include "Component/Component.h"
#include "Math/Math.h"
#include "Component/Transform.h"

using namespace Function::Comp;

FUNCTION_NAMESPACE_BEGIN

GameObject::GameObject(GameObject* InParent) : Object(EOF_IsGameObject), transform_delta_(GetMatrix4x4Identity()), transform_(this)
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
        delete component;
    }
    components_.clear();
    for (int i = sub_game_objects_.size() - 1; i >= 0; i--)
    {
        delete sub_game_objects_[i];
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
    delete component;
}

void GameObject::SetPosition(Vector3 position, bool delay)
{
    const Vector3 transform_delta = position - transform_.GetPosition();
    DeltaPosition(transform_delta, delay);
}

void GameObject::DeltaPosition(Vector3 pos_delta, bool delay)
{
    transform_delta_ = Math::Translate(GetMatrix4x4Identity(), pos_delta);
    transform_dirty_ = true;
    if (!delay)
    {
        ApplyTransformDeltas();
    }
}

void GameObject::ApplyTransformDeltas()
{
    Matrix4x4 parent_delta;
    if (parent_oject_ != nullptr)
    {
        parent_delta = parent_oject_->transform_delta_;
    }
    else
    {
        parent_delta = GetMatrix4x4Identity();
    }
    transform_delta_ = parent_delta * transform_delta_;
    for (auto* object: sub_game_objects_)
    {
        object->ApplyTransformDeltas();
    }
    transform_.ApplyModify(transform_delta_);
    transform_dirty_ = false;
    transform_delta_ = GetMatrix4x4Identity();
}

FUNCTION_NAMESPACE_END
