//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "ImGuiWindow.hpp"


class ImGuiDemoWindow : public ImGuiWindow {
    REFLECTED_CLASS(ImGuiDemoWindow)
public:
    void Draw(const ImGuiDrawer& drawer) override;
};

REGISTER_TYPE(ImGuiDemoWindow)
