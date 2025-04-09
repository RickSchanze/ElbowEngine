//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Object/Object.hpp"
#include "Window.hpp"


class ImGuiDrawer;
class ImGuiDrawWindow : public Window {
    REFLECTED_CLASS(ImGuiDrawWindow)

public:
    StringView GetWindowIdentity() override;

    virtual void Draw();
};

REGISTER_TYPE(ImGuiDrawWindow)
