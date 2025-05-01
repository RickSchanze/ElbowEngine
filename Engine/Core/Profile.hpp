//
// Created by Echo on 2025/3/21.
//
#pragma once

#include "Core/Core.hpp"
#include "String/String.hpp"
#include "TypeAlias.hpp"
#include "tracy/Tracy.hpp"
#include <source_location>

#if ENABLE_PROFILING
#define CPU_PROFILING_SCOPE ZoneScoped
#define CPU_PROFILING_SCOPE_N(Name) ZoneScopedN(Name)
#else
#define CPU_PROFILING_SCOPE
#define CPU_PROFILING_SCOPE_N(Name)
#endif

class ProfileScope
{
#if ENABLE_PROFILING
public:
    explicit ProfileScope(const char* name, const UInt32 color = 0, const std::source_location& loc = std::source_location::current())
        : zone(loc.line(), loc.file_name(), Strlen(loc.file_name()), loc.function_name(), Strlen(loc.function_name()), name, Strlen(name), color)
    {
    }

    ProfileScope(const ProfileScope&) = delete;
    ProfileScope(ProfileScope&&) = delete;

private:
    tracy::ScopedZone zone;
#else
public:
    explicit ProfileScope(const char* name, const UInt32 color = 0)
    {
    }
#endif
};
