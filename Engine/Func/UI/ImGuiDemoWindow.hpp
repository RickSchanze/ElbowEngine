//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "ImGuiWindow.hpp"


class ImGuiDemoWindow : public ImGuiWindow {
    REFLECTED_CLASS(ImGuiDemoWindow)
public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "ImGuiDemoWindow"; }
};

REGISTER_TYPE(ImGuiDemoWindow)
