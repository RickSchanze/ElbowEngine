/**
 * @file CoreGlobal.cpp
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#include "CoreGlobal.h"

void EngineStatistics::IncreaseFrameIndex()
{
    current_frame_index = (current_frame_index + 1) % graphics.parallel_render_frame_count;
}

void EngineStatistics::ResetDrawCalls()
{
    graphics.draw_calls = 0;
}

void EngineStatistics::IncreaseDrawCall(const int32_t count)
{
    graphics.draw_calls += count;
}

float GetFrameTime()
{
    return g_engine_statistics.time_delta;
}

EngineStatistics g_engine_statistics = {};
