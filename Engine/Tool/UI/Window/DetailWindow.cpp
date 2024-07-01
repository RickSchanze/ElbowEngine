/**
 * @file DetailWindow.cpp
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#include "DetailWindow.h"
#include "GameObject/GameObject.h"
#include "Object/ObjectManager.h"
#include "OutlineWindow.h"
#include "WindowManager.h"

#include <imgui.h>


#include "UI/Drawer/PropertyDrawer.h"

#include "DetailWindow.generated.h"

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

DetailWindow::DetailWindow() {
    name_       = L"Window_DetailWindow";
    mWindowName = L"细节";
}

void DetailWindow::Draw(float InDeltaTime) {
    // 存储ObjectID 并每帧查找防止使用野指针
    if (mOutlineWindow == nullptr) {
        mOutlineWindow = WindowManager::GetWindow<OutlineWindow>();
    }
    if (mOutlineWindow->IsValid()) {
        if (mOutlineWindow->SelectedObjectID != -1) {
            mSelectedObjectID = mOutlineWindow->SelectedObjectID;
        }
    }
    Function::GameObject* mObjectToDraw = nullptr;
    if (mSelectedObjectID != 0) {
        mObjectToDraw = ObjectManager::Get().GetObjectById<Function::GameObject>(mSelectedObjectID);
    }
    if (mObjectToDraw) {
        DrawSelectedObject(mObjectToDraw);
    }
}

void DetailWindow::DrawSelectedObject(Function::GameObject* InGameObject) {
    ImGui::Text(U8("对象名: %s, ID: %d"), InGameObject->GetCachedAnsiString().c_str(), InGameObject->GetID());
    Drawer::PropertyDrawer::DrawTransform(InGameObject->GetTransform());
    const auto& Components = InGameObject->GetComponents();
    for (auto* Comp : Components) {
        DrawComponent(Comp);
    }
}

void DetailWindow::DrawComponent(Function::Component* InComp) {
    if (InComp == nullptr || !InComp->IsValid()) return;
    if (ImGui::CollapsingHeader(InComp->GetCachedAnsiString().c_str())) {
        Type CompType = InComp->get_type();
        for (auto Prop : CompType.get_properties()) {
            Drawer::PropertyDrawer::DrawProperty(Prop, InComp);
        }
    }
}

WINDOW_NAMESPACE_END

