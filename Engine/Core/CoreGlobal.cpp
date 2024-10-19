/**
 * @file CoreGlobal.cpp
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#include "CoreGlobal.h"
#include "Object/Object.h"
#include "Object/ObjectManager.h"

extern Logger g_logger = {};

uint32_t ObjectCreateHelper::GetAvailableID()
{
    return ObjectManager::Get()->GetNextValidID();
}

void ObjectCreateHelper::SetObjectID(Object* Obj, const uint32_t ID)
{
    Obj->id_ = ID;
}

void ObjectCreateHelper::SetObjectID(const SharedPtr<Object>& Obj, const uint32_t ID)
{
    Obj->id_ = ID;
}

void ObjectCreateHelper::SetObjectID(const UniquePtr<Object>& Obj, const uint32_t ID)
{
    Obj->id_ = ID;
}

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

String STRING_NONE;
