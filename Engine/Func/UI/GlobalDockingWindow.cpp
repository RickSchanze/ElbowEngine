//
// Created by Echo on 2025/4/7.
//

#include "GlobalDockingWindow.hpp"

#include <imgui.h>

#include "Core/Object/ObjectManager.hpp"
#include "UIManager.hpp"

IMPL_REFLECTED(GlobalDockingWindow) { return Type::Create<GlobalDockingWindow>("GlobalDockingWindow") | refl_helper::AddParents<ImGuiWindow>(); }

void GlobalDockingWindow::Draw(const ImGuiDrawer& drawer) {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    // 全屏覆盖
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    ImGui::Begin("GlobalDockingWindow", nullptr, window_flags);
    ImGui::PopStyleVar(2);
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("窗口")) {
            if (ImGui::MenuItem("视口")) {
                UIManager::ActivateViewportWindow();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    ImGui::End();
}
