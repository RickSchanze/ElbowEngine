//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "ImGuiDrawWindow.hpp"


class GlobalDockingWindow : public ImGuiDrawWindow{
    REFLECTED_CLASS(GlobalDockingWindow)
public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "GlobalDockingWindow"; }
};

REGISTER_TYPE(GlobalDockingWindow)
