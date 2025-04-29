//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "ImGuiDrawWindow.hpp"

#include GEN_HEADER("GlobalDockingWindow.generated.hpp")

class ECLASS() GlobalDockingWindow : public ImGuiDrawWindow
{
    GENERATED_BODY(GlobalDockingWindow)
public:
    virtual void Draw() override;

    virtual StringView GetWindowIdentity() override
    {
        return "GlobalDockingWindow";
    }
};