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

UInt32 ObjectCreateHelper::GetAvailableID() {
    return ObjectManager::Get().GetNextValidID();
}

void ObjectCreateHelper::SetObjectID(Object* Obj, const UInt32 ID) {
    Obj->mID = ID;
}

void ObjectCreateHelper::SetObjectID(const TSharedPtr<Object>& Obj, const UInt32 ID) {
    Obj->mID = ID;
}

void ObjectCreateHelper::SetObjectID(const TUniquePtr<Object> &Obj, const UInt32 ID)
{
    Obj->mID = ID;
}

void EngineStatistics::IncreaseFrameIndex()
{
    current_image_index = (current_image_index + 1) % parallel_render_frame_count;
}

EngineStatistics g_engine_statistics = {};
String STRING_NONE;
