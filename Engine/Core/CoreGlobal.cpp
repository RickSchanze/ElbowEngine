/**
 * @file CoreGlobal.cpp
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#include "CoreGlobal.h"
#include "Object/Object.h"
#include "Object/ObjectManager.h"

extern Logger gLogger = {};

uint32_t ObjectCreateHelper::GetAvailableID()
{
    return ObjectManager::Get().GetNextValidID();
}

void ObjectCreateHelper::SetObjectID(Object* Obj, const uint32_t ID)
{
    Obj->id_ = ID;
}

void ObjectCreateHelper::SetObjectID(const TSharedPtr<Object>& Obj, const uint32_t ID)
{
    Obj->id_ = ID;
}

void ObjectCreateHelper::SetObjectID(const TUniquePtr<Object>& Obj, const uint32_t ID)
{
    Obj->id_ = ID;
}

void EngineStatistics::IncreaseFrameIndex()
{
    current_image_index = (current_image_index + 1) % parallel_render_frame_count;
}

void EngineStatistics::ResetDrawCalls()
{
    graphics.draw_calls = 0;
}

void EngineStatistics::IncreaseDrawCall(const uint32_t count)
{
    graphics.draw_calls += count;
}

EngineStatistics g_engine_statistics = {};

String STRING_NONE;
