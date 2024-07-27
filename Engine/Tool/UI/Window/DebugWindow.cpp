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
#include "Input/Input.h"

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

StatisticsWindow::StatisticsWindow() {
    name_       = L"Window_Statistics";
    window_name_ = L"引擎数据统计";
}

void StatisticsWindow::Draw(float InDeltaTime) {
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
    ImGui::Text(U8("当前鼠标位置:(%f, %f)"), Input::GetMousePosition().x, Input::GetMousePosition().y);
    ImGui::Text(U8("当前鼠标delta:(%f, %f)"), Input::GetMouseDelta().x, Input::GetMouseDelta().y);
    ImGui::Text(U8("Draw Call: %d"), g_engine_statistics.graphics.draw_calls);
    ImGui::Text(U8("交换链帧索引: %d"), g_engine_statistics.current_image_index);
    ImGui::Text(U8("并行渲染帧索引: %d"), g_engine_statistics.current_frame_index);
}

WINDOW_NAMESPACE_END
