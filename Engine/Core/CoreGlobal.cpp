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

uint32 ObjectCreateHelper::GetAvailableID() {
    return ObjectManager::Get().GetNextValidID();
}

void ObjectCreateHelper::SetObjectID(Object* Obj, const uint32 ID) {
    Obj->mID = ID;
}

void ObjectCreateHelper::SetObjectID(const SharedPtr<Object>& Obj, const uint32 ID) {
    Obj->mID = ID;
}

void ObjectCreateHelper::SetObjectID(const UniquePtr<Object>& Obj, const uint32 ID) {
    Obj->mID = ID;
}
