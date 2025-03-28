//
// Created by Echo on 2025/3/26.
//

#pragma once

#include <format>
#include "PlatformDef.hpp"
#include "Logger/Logger.hpp"


inline void Debug_Break() { DEBUG_BREAK(); }

template<typename... Args>
void DebugAssert(const bool condition, const std::format_string<Args...> format, Args &&...args) {
    if (!condition) {
        Log(Error) << std::format(format, std::forward<Args>(args)...);
        Debug_Break();
    }
}

template<typename... Args>
void Assert(const bool condition, const std::format_string<Args...> format, Args &&...args) {
    if (!condition) {
        Log(Fatal) << std::format(format, std::forward<Args>(args)...);
        Debug_Break();
        exit(1);
    }
}
