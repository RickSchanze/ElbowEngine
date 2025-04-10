//
// Created by Echo on 2025/4/8.
//

#include "DetailWindow.hpp"

#include "Func/World/Actor.hpp"
#include "ImGuiDrawer.hpp"
#include "InspectorWindow.hpp"
#include "UIManager.hpp"

IMPL_REFLECTED(DetailWindow) { return Type::Create<DetailWindow>("DetailWindow") | refl_helper::AddParents<ImGuiDrawWindow>(); }

void DetailWindow::Draw() {
    InspectorWindow *inspector = UIManager::GetWindow<InspectorWindow>();
    Actor *selected_actor = inspector ? inspector->GetSelectedActor() : nullptr;
    if (ImGuiDrawer::Begin("细节", &visible_)) {
        if (!selected_actor) {
            ImGui::Text("选中一个Actor以查看其细节");
        } else {
            ImGui::Text("%s", *selected_actor->GetDisplayName());
            ImGui::SameLine();
            ImGui::Text("%d", selected_actor->GetHandle());
            if (ImGui::CollapsingHeader("变换")) {
                ImGuiDrawer::DrawTransform(selected_actor);
            }
        }
    }
    ImGui::End();
}
