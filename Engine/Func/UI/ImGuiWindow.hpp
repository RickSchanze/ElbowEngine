//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Object/Object.hpp"
#include "Window.hpp"

class ImGuiWindow : public Window {
    REFLECTED_CLASS(ImGuiWindow)

public:
    ImGuiWindow();

    StringView GetWindowTitle() override;

    virtual void Draw();
};

REGISTER_TYPE(ImGuiWindow)
