/**
 * @file DebugWindow.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#include "DebugWindow.h"

#include <imgui.h>
void Tool::Window::DebugWindow::Construct() {
    WindowBase::Construct();
}

void Tool::Window::DebugWindow::Draw(float InDeltaTime) {
    WindowBase::Draw(InDeltaTime);
    ImGui::Text(U8("帧生成: %f"), InDeltaTime);
    ImGui::Text(U8("帧率: %f"), 1 / InDeltaTime);
}
