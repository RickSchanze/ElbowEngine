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
        requires(!std::is_convertible_v<T, const char *>)
    static void Concat(std::ostringstream &stream, const T &t) {
        static_assert(!NTraits::SameAs<Pure<T>, StringView>, "Did you forget to call operator *?");
        static_assert(!NTraits::SameAs<Pure<T>, String>, "Did you forget to call operator *?");
        if constexpr (NTraits::SameAs<Pure<T>, bool>) {
            stream << (t ? "true" : "false");
        } else if constexpr (std::disjunction_v<std::is_arithmetic<Pure<T>>>) {
            stream << t;
        } else if constexpr (std::is_pointer_v<Pure<T>>) {
            stream << std::hex << std::showbase << t;
        } else if constexpr (NTraits::CToStringAble<Pure<T>>) {
            stream << *t.ToString();
        } else if constexpr (NTraits::IsEnum<Pure<T>>) {
            stream << std::to_underlying(t);
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
