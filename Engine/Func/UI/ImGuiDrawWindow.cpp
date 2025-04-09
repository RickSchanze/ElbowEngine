//
// Created by Echo on 2025/4/7.
//

#include "ImGuiDrawWindow.hpp"

#include "UIManager.hpp"

IMPL_REFLECTED(ImGuiDrawWindow) {
    return Type::Create<ImGuiDrawWindow>("ImGuiDrawWindow") | refl_helper::AddParents<Window>();
}

StringView ImGuiDrawWindow::GetWindowIdentity() {
    NeverEnter();
    return "空白ImGui窗口";
}

void ImGuiDrawWindow::Draw() { NeverEnter(); }
