//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Object/Object.hpp"
#include "Window.hpp"

#include GEN_HEADER("ImGuiDrawWindow.generated.hpp")

class ImGuiDrawer;
class ECLASS() ImGuiDrawWindow : public Window {
    GENERATED_BODY(ImGuiDrawWindow)

public:
    StringView GetWindowIdentity() override;

    virtual void Draw();
};
