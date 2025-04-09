//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "ImGuiDrawWindow.hpp"


class ImGuiDemoWindow : public ImGuiDrawWindow {
    REFLECTED_CLASS(ImGuiDemoWindow)
public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "ImGuiDemoWindow"; }
};

REGISTER_TYPE(ImGuiDemoWindow)
