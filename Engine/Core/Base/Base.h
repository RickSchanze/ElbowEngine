/**
 * @file Core.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once
#include "EString.h"

template<typename T>
    requires ::std::is_enum_v<T>
core::StringView GetEnumString(T e);

template<typename T>
    requires ::std::is_enum_v<T>
constexpr int32_t GetEnumValue(const T e)
{
    return static_cast<int32_t>(e);
}

template<typename T>
    requires ::std::is_enum_v<T>
T GetEnumValue(core::StringView str);

#define ENUM_INVALID "OutOfRange"
