/**
* 一些重要的定义
*/
#pragma once
#include "Base/CoreTypeDef.h"
// 反射相关类的定义

// 定义反射宏
#include "rttr/policy.h"
#include "rttr/registration_friend"
#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#define CONCAT_IMPL3(x, y, z) x##y##z
#define MACRO_CONCAT3(x, y, z) CONCAT_IMPL3(x, y, z)

#define GENERATED_BODY(class_name) MACRO_CONCAT3(GENERATED_BODY_, CURRENT_FILE_ID, class_name)

#define GENERATED_SOURCE()                               \
    RTTR_REGISTRATION                                    \
    {                                                    \
        MACRO_CONCAT(GENERATED_SOURCE_, CURRENT_FILE_ID) \
    }

// 反射宏
#define _CONCAT2(a, b) a##b
#define CONCAT2(a, b) _CONCAT2(a, b)
#ifdef REFLECTION
#define ECLASS(...) extern void CONCAT2(REFLECTION_CLASS_TRAIT, __LINE__)(const char* param = #__VA_ARGS__);
#define ESTRUCT(...) extern void CONCAT2(REFLECTION_STRUCT_TRAIT, __LINE__)(const char* param = #__VA_ARGS__);
#define EPROPERTY(...) __attribute__((annotate("Property, " #__VA_ARGS__)))
#define EFUNCTION(...) __attribute__((annotate("Function, " #__VA_ARGS__)))
#define EENUM(...) extern void CONCAT2(REFLECTION_ENUM_TRAIT, __LINE__)(const char* param = #__VA_ARGS__);
#define EVALUE(...) __attribute__((annotate("Value, " #__VA_ARGS__)))
#else
#define ECLASS(...)
#define ESTRUCT(...)
#define EPROPERTY(...)
#define EFUNCTION(...)
#define EENUM(...)
#define EVALUE(...)
#endif

#if WITH_EDITOR
#define EDITOR_META(...) (__VA_ARGS__)
#else
#define EDITOR_META(...)
#endif

// 表示这个参数是一个输出参数
#define OUT

// 这个宏表示这个方法必须由子类实现
#define INTERFACE_METHOD \
    {                    \
    }

#define _CRT_SECURE_NO_WARNINGS

// 项目Debug宏
#define ELBOW_DEBUG _DEBUG || RELWITHDEBINFO

#define U8(Text) (const char*)u8##Text

#define INTERFACE_IMPL(type)                                     \
    RTTR_REGISTRATION                                            \
    {                                                            \
        rttr::registration::class_<type>(#type).constructor<>(); \
}

#define DEBUG_BREAK() __debugbreak()

#define REGION(region_name) 1
