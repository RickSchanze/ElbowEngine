/**
 * @file Object.cpp
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#include "Object.h"
#include "CoreGlobal.h"
#include "Log/Logger.h"
#include "ObjectManager.h"
#include "Utils/StringUtils.h"

RTTR_REGISTRATION {
    rttr::registration::class_<Object>("Object").constructor<>()(rttr::policy::ctor::as_raw_ptr)
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

void Object::SetName(const String& Name) {
    mName = Name;
    mCachedAnsiString = StringUtils::ToAnsiString(Name);
}

bool Object::IsValid() const {
    return !mIsGarbage;
}
