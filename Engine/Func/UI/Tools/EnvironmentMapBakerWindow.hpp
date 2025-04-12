//
// Created by Echo on 2025/4/12.
//

#pragma once
#include "Func/UI/ImGuiDrawWindow.hpp"


class EnvironmentMapBakerWindow : public ImGuiDrawWindow {
    REFLECTED_CLASS(EnvironmentMapBakerWindow)
public:
    void Draw() override;
};

REGISTER_TYPE(EnvironmentMapBakerWindow)
