//
// Created by Echo on 2025/4/7.
//

#include "GlobalDockingWindow.hpp"

#include <imgui.h>

#include "Core/Object/ObjectManager.hpp"
#include "DetailWindow.hpp"
#include "Func/Render/Pipeline/RenderPipeline.hpp"
#include "Func/Render/RenderContext.hpp"
#include "ImGuiDemoWindow.hpp"
#include "InspectorWindow.hpp"
#include "UIManager.hpp"

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
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("工具")) {
            if (ImGui::MenuItem("渲染管线设置")) {
                auto *pipeline = RenderContext::GetBoundRenderPipeline();
                if (pipeline) {
                    ImGuiDrawWindow *window = pipeline->GetSettingWindow();
                    if (window) {
                        window->SetVisible(true);
                    } else {
                        VLOG_ERROR("打开渲染管线设置窗口失败, 可能是因为渲染管线没有实现GetSettingWindow函数");
                    }
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    ImGui::End();
}
