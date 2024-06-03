/**
 * @file Component.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "Component.h"

#include "CoreGlobal.h"
#include "GameObject/GameObject.h"
#include "Utils/StringUtils.h"

#include "Component.generated.h"

GENERATED_SOURCE()

Function::Component::Component() = default;

Function::Component::Component(String InName, GameObject* InGameObject)
    : Object(EOF_IsComponent), mGameObject(InGameObject) {
    // Transform由GameObject负责设置
    if (InGameObject == nullptr) {
        LOG_ERROR_CATEGORY(Component, L"创建组件{}失败: 输入组件为空", InName);
        return;
    }
    SetName(InName);
    // InGameObject 由GameObject的AddComponent设置
}

void Function::Component::BeginPlay() {}

Function::Transform& Function::Component::GetTransform() const {
    return *mTransform;
}

void Function::Component::Destroy() {
    mGameObject->DestroyComponent(this);
}
