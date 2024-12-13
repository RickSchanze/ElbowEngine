//
// Created by Echo on 24-12-13.
//

#pragma once
#include "Base/EString.h"
#include "Log/Logger.h"

namespace core
{
class Assert
{
public:
    // Assert 会崩溃版本
    template <typename ...Args>
    static void Require(StringView cat, bool condition, const spdlog::format_string_t<Args...> format, Args&&... args)
    {
        if (!condition)
        {
            LOGGER.Critical(cat, format, Forward<Args>(args)...);
        }
    }

    // Assert 不会崩溃版本
    template <typename ...Args>
    static void Ensure(StringView cat, bool condition, const spdlog::format_string_t<Args...> format, Args&&... args)
    {
        if (!condition)
        {
            LOGGER.Critical(cat, format, Forward<Args>(args)...);
        }
    }
};
}