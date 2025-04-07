//
// Created by Echo on 2025/4/7.
//

#include "ImGuiDemoWindow.hpp"

#include <imgui.h>

IMPL_REFLECTED(ImGuiDemoWindow) { return Type::Create<ImGuiDemoWindow>("ImGuiDemoWindow") | refl_helper::AddParents<ImGuiWindow>(); }

void ImGuiDemoWindow::Draw(const ImGuiDrawer &drawer) { ImGui::ShowDemoWindow(&visible_); }
