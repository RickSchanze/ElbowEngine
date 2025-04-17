//
// Created by Echo on 2025/4/7.
//

#include "ImGuiDrawWindow.hpp"

#include "UIManager.hpp"

StringView ImGuiDrawWindow::GetWindowIdentity() {
    NeverEnter();
    return "空白ImGui窗口";
}

void ImGuiDrawWindow::Draw() { NeverEnter(); }
