//
// Created by Echo on 2025/4/8.
//

#pragma once
#include "ImGuiWindow.hpp"


class DetailWindow : public ImGuiWindow {
    REFLECTED_CLASS(DetailWindow)

public:
    void Draw() override;
    StringView GetWindowIdentity() override { return "DetailWindow"; }
};

REGISTER_TYPE(DetailWindow)
