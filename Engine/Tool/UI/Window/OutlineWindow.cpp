/**
 * @file OutlineWindow.cpp
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#include "OutlineWindow.h"
#include <imgui.h>

#include "GameObject/GameObject.h"
#include "OutlineWindow.generated.h"

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

static bool TreeNodeHasSelected = false;

OutlineWindow::OutlineWindow() {
    name_       = L"Window_OutlineWindow";
    mWindowName = L"世界大纲";
}

void OutlineWindow::Draw(float InDeltaTime) {
    auto Objs = Function::GameObject::GetRootGameObjects();
    for (auto& Obj: Objs) {
        DrawGameObject(Obj);
    }
    TreeNodeHasSelected = false;
}

void OutlineWindow::DrawGameObject(Function::GameObject* InGameObject) {
    if (InGameObject == nullptr) return;
    ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (!InGameObject->HasChildren()) {
        Flags |= ImGuiTreeNodeFlags_Leaf;
    }
    // TODO: 递归改循环
    if (ImGui::TreeNodeEx(InGameObject->GetCachedAnsiString().c_str(), Flags)) {
        if (ImGui::IsItemClicked()) {
            SelectedObjectID = InGameObject->GetID();
        }
        for (auto& Child: InGameObject->GetChildren()) {
            DrawGameObject(Child);
        }
        ImGui::TreePop();
    }
}

WINDOW_NAMESPACE_END
