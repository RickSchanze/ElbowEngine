/**
 * 一些重要的定义
 */
#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Features.h"

#if WITH_EDITOR
#define EDITOR_META(...) (__VA_ARGS__)
#else
#define EDITOR_META(...)
#endif

// 表示这个参数是一个输出参数
#define OUT

// 这个宏表示这个方法必须由子类实现
#define INTERFACE_METHOD                                                                                               \
  {                                                                                                                    \
  }

#define _CRT_SECURE_NO_WARNINGS

// 项目Debug宏
#define ELBOW_DEBUG _DEBUG || RELWITHDEBINFO

#define U8(Text) (const char *)u8##Text

#define INTERFACE_IMPL(type)                                                                                           \
  RTTR_REGISTRATION { rttr::registration::class_<type>(#type).constructor<>(); }

#define DEBUG_BREAK() __debugbreak()

#define NEVER_ENTRY_WARN(cat) LOGGER.Warn(cat, "此方法未实现: {}", __FUNCSIG__)

#define REGION(region_name) 1

#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#elif defined(__linux__)
#define PLATFORM_LINUX
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#define PLATFORM_APPLE
#endif
#if TARGET_OS_IPHONE
#define PLATFORM_IOS
#endif
#elif defined(__ANDROID__)
#define PLATFORM_ANDROID
#else
#error "Unknown platform"
#endif

#define BREAKABLE_SCOPE                                                                                                \
  for (int BREAKABLE_SCOPE_VARIABLE__ = 0; BREAKABLE_SCOPE_VARIABLE__ < 1; BREAKABLE_SCOPE_VARIABLE__++)

#if ELBOW_DEBUG
#define DEBUG_ONLY(x) x
#define DEBUG_ONLY_PARAM(x) , x
#else
#define DEBUG_ONLY(x)
#define DEBUG_ONLY_PARAM(x)
#endif

bool ValidateFeatureState();

void NoEntry();

#define NAMEOF(x) #x
