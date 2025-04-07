//
// Created by Echo on 2025/4/8.
//

#pragma once
#include "ImGuiDemoWindow.hpp"

// 只是一些便于本项目的imgui默认值wraper
class ImGuiDrawer {
public:
    static bool Begin(const char *title, bool *p_open = nullptr, ImGuiWindowFlags f = ImGuiWindowFlags_NoCollapse);
    static void End();
};
