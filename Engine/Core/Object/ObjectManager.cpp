/**
 * @file ObjectManager.cpp
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#include "ObjectManager.h"
#include "CoreGlobal.h"
#include "Object.h"

bool ObjectManager::AddObject(Object* NewObject) {
    if (!IsValid(NewObject)) return false;
    if (IsIDValid(NewObject->GetID())) {
        mObjects[NewObject->GetID()] = NewObject;
        return true;
    } else {
        auto ValidID = GetNextValidID();
        ObjectCreateHelper::SetObjectID(NewObject, ValidID);
        mObjects[ValidID] = NewObject;
        return true;
    }
    return false;
}

bool ObjectManager::IsIDValid(const UInt32 ID) const {
    return ID != 0 && !mObjects.contains(ID);
}

UInt32 ObjectManager::GetNextValidID() {
    while (!IsIDValid(mIDCount++)) {}
    return mIDCount;
}

bool ObjectManager::RemoveObject(const UInt32 ID) {
    if (mObjects.erase(ID) != 0) {
        return true;
    }
    return false;
}
ObjectManager::~ObjectManager() {
    while (!mObjects.empty()) {
        delete mObjects.begin()->second;
    }
    mObjects.clear();
}
