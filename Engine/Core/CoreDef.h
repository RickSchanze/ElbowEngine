/**
* 一些重要的定义
*/
#pragma once
#include "Core/Base/CoreTypeDef.h"

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

#define NEVER_ENTRY_WARN(cat) LOGGER.Warn(cat, "此方法未实现: {}", __FUNCSIG__)

#define REGION(region_name) 1


#ifdef ELBOW_DEBUG
#define DebugAssert(log_cat, expr, msg, ...)        \
    if (!(expr))                                    \
    {                                               \
        LOGGER.Critical(log_cat, msg, __VA_ARGS__); \
        DEBUG_BREAK();                              \
    }

#define Assert(log_cat, expr, msg, ...)             \
    if (!(expr))                                    \
    {                                               \
        LOGGER.Critical(log_cat, msg, __VA_ARGS__); \
        DEBUG_BREAK();                              \
        std::terminate();                           \
    }
#else
#define DebugAssert(log_cat, expr, msg, ...)
#define Assert(log_cat, expr, msg, ...)             \
    if (!(expr))                                    \
    {                                               \
        LOGGER.Critical(log_cat, msg, __VA_ARGS__); \
        std::terminate();                           \
    }
#endif

