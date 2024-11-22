/**
 * @file Core.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once
#include "EString.h"
#include "Core/Reflection/Reflection.h"

template<typename T>
    requires ::std::is_enum_v<T>
core::StringView GetEnumString(T e)
{
    const core::Type* type = core::TypeOf<T>();
    Assert(logcat::Core, type != nullptr && type->IsEnum(), "Type not valid");
    for (auto field: type->GetSelfDefinedFields())
    {
        Assert(logcat::Core, type != nullptr && type->IsEnum(), "Type is a enum but its fields are not enum values");
        if (field->GetEnumFieldValue() == static_cast<int32_t>(e))
        {
            return field->GetName();
        }
    }
    return "ENUM_INVALID";
}

template<typename T>
    requires ::std::is_enum_v<T>
constexpr int32_t GetEnumValue(const T e)
{
    return static_cast<int32_t>(e);
}

template<typename T>
    requires ::std::is_enum_v<T>
T GetEnumValue(core::StringView str)
{
    const core::Type* type = core::TypeOf<T>();
    Assert(logcat::Core, type != nullptr && type->IsEnum(), "Type not valid");
    for (auto field: type->GetSelfDefinedFields())
    {
        Assert(logcat::Core, type != nullptr && type->IsEnum(), "Type is a enum but its fields are not enum values");
        if (field->GetName() == str)
        {
            return static_cast<T>(field->GetEnumFieldValue());
        }
    }
    return T::Count;
}

#define ENUM_INVALID "OutOfRange"
