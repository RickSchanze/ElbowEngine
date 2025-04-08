//
// Created by Echo on 2025/4/7.
//

#include "ImGuiWindow.hpp"

#include "UIManager.hpp"

IMPL_REFLECTED(ImGuiWindow) {
    return Type::Create<ImGuiWindow>("ImGuiWindow") | refl_helper::AddParents<Window>();
}

StringView ImGuiWindow::GetWindowIdentity() {
    NeverEnter();
    return "空白ImGui窗口";
}

void ImGuiWindow::Draw() { NeverEnter(); }
