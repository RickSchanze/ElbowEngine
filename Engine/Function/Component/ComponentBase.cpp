/**
 * @file ComponentBase.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "ComponentBase.h"

#include "CoreGlobal.h"
#include "GameObject/GameObject.h"

Function::ComponentBase::ComponentBase(String InName, GameObject* InGameObject) : mGameObject(InGameObject) {
    if (InGameObject == nullptr) {
        LOG_ERROR_CATEGORY(Component, L"创建组件{}失败: 输入组件为空", InName);
        return;
    }
    mName = InName;
    // InGameObject 由GameObject的AddComponent设置
}

void Function::ComponentBase::BeginPlay() {

}

void Function::ComponentBase::Destroy() {
    mGameObject->DestroyComponent(this);
}
