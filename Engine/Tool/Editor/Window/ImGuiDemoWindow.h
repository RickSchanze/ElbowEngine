/**
 * @file ImGuiDemoWindow.h
 * @author Echo 
 * @Date 24-8-15
 * @brief 
 */

#pragma once
#include "WindowBase.h"

#include "ImGuiDemoWindow.generated.h"

WINDOW_NAMESPACE_BEGIN

class REFL ImGuiDemoWindow : public WindowBase {
    GENERATED_BODY(ImGuiDemoWindow)
public:
    ImGuiDemoWindow();
    void Draw(float InDeltaTime) override;
};

WINDOW_NAMESPACE_END
