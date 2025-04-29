//
// Created by Echo on 2025/4/8.
//

#pragma once
#include "ImGuiDrawWindow.hpp"

#include GEN_HEADER("DetailWindow.generated.hpp")

class ECLASS() DetailWindow : public ImGuiDrawWindow
{
    GENERATED_BODY(DetailWindow)

public:
    virtual void Draw() override;

    virtual StringView GetWindowIdentity() override
    {
        return "DetailWindow";
    }
};
