/**
 * @file Object.cpp
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#include "Object.h"
#include "Log/Logger.h"
#include "Serialization/Archive.h"

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

void Object::Serialize(Archive& ar)
{
    // Type t = GetType();
    // ar << Archive::InputType::MapStart;
    // ar << Archive::InputType::Key;
    // StringView type_name = t.get_name().data();
    // ar << type_name;
    // ar << Archive::InputType::Value;
    // {
    //     ar << Archive::InputType::MapStart;
    //     const auto& properties = t.get_properties();
    //     for (const auto& prop: properties)
    //     {
    //         if (prop.get_type().is_pointer())
    //         {
    //             continue;
    //         }
    //         String name = {prop.get_name().data(), static_cast<int32_t>(prop.get_name().length())};
    //         ar << Archive::InputType::Key;
    //         ar << name;
    //         ar << Archive::InputType::Value;
    //         ar << prop.get_value(*this);
    //     }
    //     ar << Archive::InputType::MapEnd;
    // }
    // ar << Archive::InputType::MapEnd;
}
}   // namespace core
