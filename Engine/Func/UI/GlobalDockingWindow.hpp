//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "ImGuiWindow.hpp"


class GlobalDockingWindow : public ImGuiWindow{
    REFLECTED_CLASS(GlobalDockingWindow)
public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "GlobalDockingWindow"; }
};

REGISTER_TYPE(GlobalDockingWindow)
