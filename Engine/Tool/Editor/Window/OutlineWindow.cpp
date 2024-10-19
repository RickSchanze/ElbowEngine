/**
 * @file OutlineWindow.cpp
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#include "OutlineWindow.h"
#include "ImGui/ImGuiHelper.h"
#include "GameObject/GameObject.h"

#include "OutlineWindow.generated.h"

GENERATED_SOURCE()

namespace tool::window {

static bool TreeNodeHasSelected = false;

OutlineWindow::OutlineWindow()
{
    name_       = L"Window_OutlineWindow";
    window_name_ = L"世界大纲";
}

void OutlineWindow::Draw(float InDeltaTime)
{
    auto objs = function::GameObject::GetRootGameObjects();
    for (auto& obj: objs)
    {
        DrawGameObject(obj);
    }
    TreeNodeHasSelected = false;
}

void OutlineWindow::DrawGameObject(function::GameObject* game_object)
{
    if (game_object == nullptr) return;
    int flags = EImGuiTNF_OpenOnArrow | EImGuiTNF_OpenOnDoubleClick;
    if (!game_object->HasChildren())
    {
        flags |= EImGuiTNF_Leaf;
    }
    // TODO: 递归改循环
    if (ImGuiHelper::TreeNodeEx(game_object->GetCachedAnsiString().c_str(), flags))
    {
        if (ImGuiHelper::IsItemClicked())
        {
            selected_object_id = game_object->GetID();
        }
        for (auto& child: game_object->GetChildren())
        {
            DrawGameObject(child);
        }
        ImGuiHelper::TreePop();
    }
}

}
