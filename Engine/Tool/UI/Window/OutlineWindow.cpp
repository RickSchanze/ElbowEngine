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

OutlineWindow::OutlineWindow()
{
    name_       = L"Window_OutlineWindow";
    mWindowName = L"世界大纲";
}

void OutlineWindow::Draw(float InDeltaTime)
{
    auto objs = Function::GameObject::GetRootGameObjects();
    for (auto& obj: objs)
    {
        DrawGameObject(obj);
    }
    TreeNodeHasSelected = false;
}

void OutlineWindow::DrawGameObject(Function::GameObject* game_object)
{
    if (game_object == nullptr) return;
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (!game_object->HasChildren())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }
    // TODO: 递归改循环
    if (ImGui::TreeNodeEx(game_object->GetCachedAnsiString().c_str(), flags))
    {
        if (ImGui::IsItemClicked())
        {
            selected_object_id = game_object->GetID();
        }
        for (auto& child: game_object->GetChildren())
        {
            DrawGameObject(child);
        }
        ImGui::TreePop();
    }
}

WINDOW_NAMESPACE_END
