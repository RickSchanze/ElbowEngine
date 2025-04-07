//
// Created by Echo on 2025/4/8.
//

#include "ImGuiDrawer.hpp"
bool ImGuiDrawer::Begin(const char *title, bool *p_open, ImGuiWindowFlags f) { return ImGui::Begin(title, p_open, f); }
void ImGuiDrawer::End() { ImGui::End(); }
