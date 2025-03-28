//
// Created by Echo on 25-3-19.
//

#pragma once

#include <string>

#include "CoreFeature.hpp"

// 检测 Windows 平台（包括 32/64位）
#if defined(_WIN32) || defined(_WIN64)
#define ELBOW_PLATFORM_WINDOWS
// 检测 Android（需要放在 Linux 之前，因为 Android 也定义 __linux__）
#elif defined(__ANDROID__)
#define ELBOW_PLATFORM_ANDROID
// 检测 Linux
#elif defined(__linux__)
#define ELBOW_PLATFORM_LINUX
#else
#error "Unknow platform!"
#endif

#ifdef ELBOW_DEBUG
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK()
#endif
