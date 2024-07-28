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
    ObjectManager::Get()->RemoveObject(id_);
    LOG_INFO_CATEGORY(Object, L"删除对象: {}, ID: {}", name_, id_);
}

String Object::ToString() const {
    return std::format(L"[Object] Name: {0}, Id: {1}", name_, id_);
}

void Object::SetName(const String& name) {
    name_ = name;
    cached_ansi_string_ = StringUtils::ToAnsiString(name);
}

bool Object::IsValid() const
{
    return !is_garbage_;
}

const AnsiString& Object::GetCachedAnsiString(){
    if (cached_ansi_string_.empty())
    {
        cached_ansi_string_ = StringUtils::ToAnsiString(name_);
    }
    return cached_ansi_string_;
}
