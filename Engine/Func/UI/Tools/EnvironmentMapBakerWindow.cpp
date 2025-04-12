//
// Created by Echo on 2025/4/12.
//

#include "EnvironmentMapBakerWindow.hpp"

IMPL_REFLECTED(EnvironmentMapBakerWindow) {
    return Type::Create<EnvironmentMapBakerWindow>("EnvironmentMapBakerWindow") | refl_helper::AddParents<ImGuiDrawWindow>();
}

void EnvironmentMapBakerWindow::Draw() { ImGuiDrawWindow::Draw(); }
