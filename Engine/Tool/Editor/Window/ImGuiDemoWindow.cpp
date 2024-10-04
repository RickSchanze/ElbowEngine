/**
 * @file ImGuiDemoWindow.cpp
 * @author Echo 
 * @Date 24-8-15
 * @brief 
 */

#include "ImGuiDemoWindow.h"

#include "ImGui/ImGuiHelper.h"

#include "ImGuiDemoWindow.generated.h"

GENERATED_SOURCE()

namespace tool::window {

ImGuiDemoWindow::ImGuiDemoWindow()
{
    name_        = L"ImGuiDemoWindow";
    window_name_ = L"ImGui Demo窗口";
}

void ImGuiDemoWindow::Draw(float InDeltaTime)
{
    ImGuiHelper::ShowDemoWindow();
}
}
