/**
 * @file ProfileMacro.h
 * @author Echo 
 * @Date 24-9-15
 * @brief 
 */

#pragma once

#include "CoreMacro.h"

#ifdef ENABLE_PROFILING
#include "tracy/Tracy.hpp"

uint32_t GetColor(int index);

struct ColorScoped
{
    static inline int32_t level = 0;

    ColorScoped()
    {
        level++;
    }

    ~ColorScoped()
    {
        level--;
    }
};

static uint32_t GetColor()
{
    return GetColor(ColorScoped::level);
}

#define COLOR_SCOPED ColorScoped __color_scope##__LINE__;
#define PROFILE_SCOPE_AUTO ZoneScoped; COLOR_SCOPED ZoneColor(GetColor());
#define PROFILE_SCOPE(name) ZoneScopedN(name); COLOR_SCOPED ZoneColor(GetColor());
#define MARK_FRAME(name) FrameMarkNamed(name)
#define MARK_FRAME_AUTO FrameMark
#else
#define PROFILE_SCOPE_AUTO
#define PROFILE_SCOPE(name)
#define MARK_FRAME(name)
#define MARK_FRAME_AUTO
#endif
