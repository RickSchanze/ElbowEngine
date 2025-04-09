//
// Created by Echo on 2025/4/8.
//

#pragma once
#include "ImGuiDrawWindow.hpp"


class DetailWindow : public ImGuiDrawWindow {
    REFLECTED_CLASS(DetailWindow)

public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "DetailWindow"; }
};

REGISTER_TYPE(DetailWindow)
