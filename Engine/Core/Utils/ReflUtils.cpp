/**
 * @file TypeUtils.cpp
 * @author Echo 
 * @Date 24-4-8
 * @brief 
 */

#include "ReflUtils.h"

AnsiString
ReflUtils::GetPropertyAttribute(const rttr::property& Property, const AnsiString& AttrName) {
    if (!Property.is_valid()) return "";
    if (AttrName.empty()) return "";
    return Property.get_metadata(AttrName).to_string();
}

bool ReflUtils::CheckAttribute(const rttr::property& Property, const AnsiString& AttrName) {
    const auto& AttrValue = GetPropertyAttribute(Property, AttrName);
    return AttrValue == "True";
}

bool ReflUtils::IsPropertySerializable(const rttr::property& Property) {
    return CheckAttribute(Property, "Serialized");
}

bool ReflUtils::IsPropertySerializedByReference(const rttr::property& Property) {
    return CheckAttribute(Property, "RefSerialized");
}
