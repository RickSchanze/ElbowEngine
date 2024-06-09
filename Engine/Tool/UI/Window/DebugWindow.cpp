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
        mRecordedFps       = gEngineStatistics.Fps;
        mRecordedDeltaTime = InDeltaTime;
        mFpsRefreshTime = 0.f;
    }
    ImGui::Text(U8("帧生成: %f"), mRecordedDeltaTime);
    ImGui::Text(U8("帧率: %d"), mRecordedFps);
    ImGui::Text(U8("总对象数: %d"), gEngineStatistics.ObjectCount);
    ImGui::Text(U8("窗口宽高: (%d, %d)"), gEngineStatistics.WindowSize.Width, gEngineStatistics.WindowSize.Height);
    ImGui::Text(U8("总帧数: %ld"), gEngineStatistics.FrameCount);
}

WINDOW_NAMESPACE_END
