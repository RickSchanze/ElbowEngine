/**
 * @file Object.cpp
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#include "Object.h"
#include "Log/Logger.h"
#include "Serialization/Archive.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<core::Object>("core::Object")
        .constructor<>()(rttr::policy::ctor::as_raw_ptr)
        .property("name_", &core::Object::name_)
        .property("id_", &core::Object::id_)
        .property("flag_", &core::Object::flag_);

        // clang-format off
    rttr::registration::enumeration<core::ObjectCategory>("core::ObjectCategory")(
        rttr::value("PureObject", core::ObjectCategory::PureObject),
        rttr::value("GameObject", core::ObjectCategory::GameObject),
        rttr::value("Window", core::ObjectCategory::Window)
    );
    // clang-format on
}

namespace core
{

Object::Object()
{
    flag_ = PureObject;
    GeneratedID();
    name_ = fmt::format("{} Object {}", GetEnumString(flag_), id_);
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
    Type t = GetType();
    ar << Archive::InputType::MapStart;
    ar << Archive::InputType::Key;
    StringView type_name = t.get_name().data();
    ar << type_name;
    ar << Archive::InputType::Value;
    {
        ar << Archive::InputType::MapStart;
        const auto& properties = t.get_properties();
        for (const auto& prop: properties)
        {
            if (prop.get_type().is_pointer())
            {
                continue;
            }
            String name = {prop.get_name().data(), static_cast<int32_t>(prop.get_name().length())};
            ar << Archive::InputType::Key;
            ar << name;
            ar << Archive::InputType::Value;
            ar << prop.get_value(*this);
        }
        ar << Archive::InputType::MapEnd;
    }
    ar << Archive::InputType::MapEnd;
}
}   // namespace core
