//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "ImGuiWindow.hpp"


class GlobalDockingWindow : public ImGuiWindow{
    REFLECTED_CLASS(GlobalDockingWindow)
public:
    void Draw(const ImGuiDrawer& drawer) override;
};

REGISTER_TYPE(GlobalDockingWindow)
