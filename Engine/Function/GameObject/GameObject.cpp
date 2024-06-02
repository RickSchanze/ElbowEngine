/**
 * @file GameObject.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "GameObject.h"
#include "Component/Component.h"

void Function::GameObject::BeginPlay() {
    // 对于已经注册的进行BeginPlay
    for (const auto Component: mComponents) {
        Component->BeginPlay();
    }
}

void Function::GameObject::Tick(float DeltaTime) const {
    for (const auto Component: mComponents) {
        Component->Tick(DeltaTime);
    }
}

void Function::GameObject::EndPlay() {
    for (const auto Component: mComponents) {
        Component->EndPlay();
    }
}

void Function::GameObject::DestroyComponent(Component* InComponent) {
    InComponent->EndPlay();
    std::erase(mComponents, InComponent);
    delete InComponent;
}
