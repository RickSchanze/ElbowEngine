/**
 * @file Object.cpp
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#include "Core.h"
#include "Object.h"
#include "Log/Logger.h"
#include "Reflection/Reflection.h"
#include "Serialization/Archive.h"

#include "Core.Object.generated.h"

GENERATED_SOURCE()

namespace core
{

Object::Object()
{
    flag_ = PureObject;
    GeneratedID();
    // name_ = fmt::format("{} Object {}", GetEnumString(flag_), id_);
}

void Object::GeneratedID()
{
    id_ = ++s_id_counter_;
}

Object::~Object() = default;

String Object::ToString() const
{
    return name_;
}

void Object::SetName(const String& name)
{
    name_ = name;
}
}   // namespace core
