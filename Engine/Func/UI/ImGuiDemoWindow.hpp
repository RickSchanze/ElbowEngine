//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "ImGuiDrawWindow.hpp"

#include GEN_HEADER("ImGuiDemoWindow.generated.hpp")


class ECLASS() ImGuiDemoWindow : public ImGuiDrawWindow {
    GENERATED_BODY(ImGuiDemoWindow)
public:
    virtual void Draw() override;
    virtual StringView GetWindowIdentity() override { return "ImGuiDemoWindow"; }
};
