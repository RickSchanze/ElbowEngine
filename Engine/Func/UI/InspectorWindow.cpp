//
// Created by Echo on 2025/4/8.
//

#include "InspectorWindow.hpp"

#include "Func/World/Actor.hpp"
#include "Func/World/Scene/Scene.hpp"
#include "ImGuiDrawer.hpp"

void InspectorWindow::Draw() {
    static bool a = true;
    static float b[3];
    if (ImGuiDrawer::Begin("Inspector", &mVisible)) {
        ImGui::BeginTable("##Actors", 1);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        for (auto *actor: Scene::GetByRef().GetActors()) {
            ImGui::PushID(actor);
            if (ImGui::TreeNode(*actor->GetDisplayName())) {
                ImGui::TreePop();
                selected_actor_ = actor;
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
