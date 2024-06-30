/**
 * @file DebugWindow.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */



#include <imgui.h>

#include "DebugWindow.h"

#include <iostream>

#include "DebugWindow.generated.h"

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

StatisticsWindow::StatisticsWindow() {
    mName       = L"Window_Statistics";
    mWindowName = L"引擎数据统计";
}

void StatisticsWindow::Draw(float InDeltaTime) {
    WindowBase::Draw(InDeltaTime);
    mFpsRefreshTime += InDeltaTime;
    if (mFpsRefreshTime > 1.f) {
        mRecordedFps       = g_engine_statistics.fps;
        mRecordedDeltaTime = InDeltaTime;
        mFpsRefreshTime = 0.f;
    }
    ImGui::Text(U8("帧生成: %f"), mRecordedDeltaTime);
    ImGui::Text(U8("帧率: %d"), mRecordedFps);
    ImGui::Text(U8("总对象数: %d"), g_engine_statistics.object_count);
    ImGui::Text(U8("窗口宽高: (%d, %d)"), g_engine_statistics.window_size.width, g_engine_statistics.window_size.height);
    ImGui::Text(U8("总帧数: %ld"), g_engine_statistics.frame_count);
}

WINDOW_NAMESPACE_END
