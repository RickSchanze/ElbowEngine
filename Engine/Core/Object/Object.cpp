/**
 * @file Object.cpp
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#include "Object.h"
#include "Core/CoreGlobal.h"
#include "Core/Log/Logger.h"
#include "ObjectManager.h"

RTTR_REGISTRATION {
    rttr::registration::class_<Object>("Object")
        .property("Name", &Object::mName)(rttr::metadata("Serialized", "True"))
        .property("Id", &Object::mID)(rttr::metadata("Serialized", "True"));
}

Object::~Object() {
    ObjectManager::Get().RemoveObject(mID);
    gLogger.Info(L"Delete Object {}", mName);
}

String Object::ToString() const {
    return std::format(L"[Object] Name: {0}, Id: {1}", mName, mID);
}

bool Object::Valid() const {
    return !mIsGarbage;
}
