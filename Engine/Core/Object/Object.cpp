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
        .property("Name", &Object::name_)(rttr::metadata("Serialized", "True"))
        .property("Id", &Object::id_)(rttr::metadata("Serialized", "True"));
}

Object::~Object() {
    ObjectManager::Get().RemoveObject(id_);
    gLogger.Info(L"Delete Object {}", name_);
}

String Object::ToString() const {
    return std::format(L"[Object] Name: {0}, Id: {1}", name_, id_);
}

void Object::SetName(const String& name) {
    name_ = name;
    cached_ansi_string_ = StringUtils::ToAnsiString(name);
}

bool Object::IsValid() const {
    return !is_garbage_;
}
