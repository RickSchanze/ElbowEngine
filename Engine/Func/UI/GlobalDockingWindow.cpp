//
// Created by Echo on 2025/4/7.
//

#include "GlobalDockingWindow.hpp"

#include <imgui.h>

#include "Core/Object/ObjectManager.hpp"
#include "DetailWindow.hpp"
#include "ImGuiDemoWindow.hpp"
#include "InspectorWindow.hpp"
#include "Platform/Window/TestFunctionWindow.hpp"
#include "UIManager.hpp"

IMPL_REFLECTED(GlobalDockingWindow) { return Type::Create<GlobalDockingWindow>("GlobalDockingWindow") | refl_helper::AddParents<ImGuiDrawWindow>(); }

void GlobalDockingWindow::Draw() {
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
                UIManager::CreateOrActivateWindow<ViewportWindow>();
            }
            if (ImGui::MenuItem("Inspector")) {
                UIManager::CreateOrActivateWindow<InspectorWindow>();
            }
            if (ImGui::MenuItem("ImGui Demo")) {
                UIManager::CreateOrActivateWindow<ImGuiDemoWindow>();
            }
            if (ImGui::MenuItem("细节")) {
                UIManager::CreateOrActivateWindow<DetailWindow>();
            }
            if (ImGui::MenuItem("功能测试")) {
                UIManager::CreateOrActivateWindow<TestFunctionWindow>();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    ImGui::End();
}
