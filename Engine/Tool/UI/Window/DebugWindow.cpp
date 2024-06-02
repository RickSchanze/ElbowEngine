/**
 * @file DebugWindow.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#include "DebugWindow.h"

#include <imgui.h>

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

DebugWindow::DebugWindow() {
    mName = L"Window_DebugWindow";
}

void DebugWindow::Construct() {
    WindowBase::Construct();
}

void DebugWindow::Draw(float InDeltaTime) {
    WindowBase::Draw(InDeltaTime);
    ImGui::Text(U8("帧生成: %f"), InDeltaTime);
    ImGui::Text(U8("帧率: %f"), 1 / InDeltaTime);
}

WINDOW_NAMESPACE_END
