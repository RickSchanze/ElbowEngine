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


#include "Component/Component.h"
#include "Component/Interface/IDetailGUIDrawer.h"
#include "Editor/Drawer/PropertyDrawer.h"

#include "DetailWindow.generated.h"
#include "ImGui/ImGuiHelper.h"
#include "Utils/ReflUtils.h"

GENERATED_SOURCE()

namespace tool::window
{

DetailWindow::DetailWindow()
{
    name_        = L"Window_DetailWindow";
    window_name_ = L"细节";
}

void DetailWindow::Draw(float delta_time)
{
    // 存储ObjectID 并每帧查找防止使用野指针
    if (outline_window_ == nullptr)
    {
        outline_window_ = WindowManager::GetOrCreateWindow<OutlineWindow>();
    }
    if (outline_window_->IsValid())
    {
        if (outline_window_->selected_object_id != -1)
        {
            selected_object_id_ = outline_window_->selected_object_id;
        }
    }
    Function::GameObject* object_to_draw = nullptr;
    if (selected_object_id_ != 0)
    {
        object_to_draw = ObjectManager::Get()->GetObjectById<Function::GameObject>(selected_object_id_);
    }
    if (object_to_draw)
    {
        DrawSelectedObject(object_to_draw);
    }
}

void DetailWindow::DrawSelectedObject(Function::GameObject* game_object)
{
    ImGui::Text(U8("对象名: %s, ID: %d"), game_object->GetCachedAnsiString().c_str(), game_object->GetID());
    drawer::PropertyDrawer::DrawTransform(game_object->GetTransform());
    const auto& components = game_object->GetComponents();
    for (auto* comp: components)
    {
        DrawComponent(comp);
    }
}

void DetailWindow::DrawComponent(Function::Comp::Component* comp)
{
    if (comp == nullptr || !comp->IsValid()) return;
    if (comp->IsImplemented<Function::IDetailGUIDrawer>())
    {
        auto* drawer = comp->As<Function::IDetailGUIDrawer>();
        if (drawer != nullptr)
        {
            drawer->OnInspectorGUI();
            return;
        }
    }
    if (ImGui::CollapsingHeader(comp->GetCachedAnsiString().c_str()))
    {
        Type comp_type = comp->get_type();
        for (auto prop: comp_type.get_properties())
        {
            if (prop.get_name() == "Id") continue;
            if (ReflUtils::GetPropertyAttribute(prop, "Label") == comp->GetCachedAnsiString())
            {
                ImGuiHelper::WarningBox(U8("这个属性与组件本身名称相同，这可能导致对这个属性的修改不生效"));
            }
            drawer::PropertyDrawer::DrawProperty(prop, comp);
        }
    }
}

}

