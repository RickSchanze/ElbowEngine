//
// Created by Echo on 2025/3/27.
//

#pragma once

#include <source_location>
#include <sstream>
#include <type_traits>
#include "Core/TypeTraits.hpp"
#include "Logger.hpp"

class VLog {
public:
    template<typename T>
        requires(!std::same_as<T, const char *>)
    static void Concat(std::ostringstream &stream, T &t) {
        if constexpr (std::disjunction_v<std::is_arithmetic<Pure<T>>>) {
            stream << t;
        } else if constexpr (std::disjunction_v<std::is_same<Pure<T>, String>, std::is_same<Pure<T>, StringView>>) {
            stream << *t;
        } else if constexpr (std::is_pointer_v<Pure<T>>) {
            stream << std::hex << std::showbase << t;
        } else if constexpr (CToStringAble<Pure<T>>) {
            stream << *t.ToString();
        } else {
            static_assert(false, "T Can not Stringify.");
        }
    }

    static void Concat(std::ostringstream &stream, const char *str) { stream << str; }

    template<typename... Args>
    VLog(const std::source_location &loc, const LogLevel level, Args &&...args) {
        std::ostringstream stream;
        (Concat(stream, Forward<Args>(args)), ...);
        Log(level, loc) << stream.str();
    }
};

#define VLOG_INFO(...) VLog(std::source_location::current(), Info, __VA_ARGS__)
#define VLOG_WARN(...) VLog(std::source_location::current(), Warn, __VA_ARGS__)
#define VLOG_ERROR(...) VLog(std::source_location::current(), Error, __VA_ARGS__)
#define VLOG_FATAL(...) VLog(std::source_location::current(), Fatal, __VA_ARGS__)
