/**
 * @file GameObject.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "GameObject.h"
#include "Component/Component.h"

using namespace Function::Comp;

Function::GameObject::GameObject(GameObject* InParent) {
    if (InParent == nullptr) {
        s_root_objects_.push_back(this);
    } else {
        parent_oject_ = InParent;
        InParent->sub_game_objects_.push_back(this);
    }
}

Function::GameObject::~GameObject() {
    for (auto component: components_) {
        delete component;
    }
    for (auto game_object: sub_game_objects_) {
        delete game_object;
    }
    if (parent_oject_ != nullptr) {
        std::erase(parent_oject_->sub_game_objects_, this);
    } else {
        std::erase(s_root_objects_, this);
    }
}

void Function::GameObject::BeginPlay() {
    // 对于已经注册的进行BeginPlay
    for (const auto Component: components_) {
        Component->BeginPlay();
    }
}

void Function::GameObject::Tick(float delta_time) const {
    for (const auto Component: components_) {
        Component->Tick(delta_time);
    }
}

void Function::GameObject::EndPlay() {
    for (const auto Component: components_) {
        Component->EndPlay();
    }
}

void Function::GameObject::DestroyComponent(Component* component) {
    component->EndPlay();
    std::erase(components_, component);
    delete component;
}
