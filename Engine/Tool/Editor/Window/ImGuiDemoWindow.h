/**
 * @file ImGuiDemoWindow.h
 * @author Echo 
 * @Date 24-8-15
 * @brief 
 */

#pragma once
#include "WindowBase.h"

#include "ImGuiDemoWindow.generated.h"

namespace tool::window {

ECLASS()
class ImGuiDemoWindow : public WindowBase {
    GENERATED_CLASS(ImGuiDemoWindow)
public:
    ImGuiDemoWindow();
    void Draw(float InDeltaTime) override;
};

}
