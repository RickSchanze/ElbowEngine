/**
 * @file ProfileMacro.cpp
 * @author Echo 
 * @Date 24-9-15
 * @brief 
 */

#include "ProfileMacro.h"

#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Core/CoreDef.h"
#include "Core/Math/Math.h"

uint32_t GetColor(int index)
{
    core::StaticArray colors = {
        0xff4500,   // OrangeRed
        0xff6347,   // Tomato
        0xff7f50,   // Coral
        0xff8c00,   // DarkOrange
        0xdc143c,   // Crimson
        0xb22222,   // Firebrick
        0x8b0000,   // DarkRed
        0xa52a2a,   // Brown
        0xd2691e,   // Chocolate
        0x8b4513,   // SaddleBrown
    };
    return colors[index % colors.size()];
}

void StartupProfiler()
{
#if ENABLE_PROFILING
    auto* cfg = core::GetConfig<core::CoreConfig>();
#endif
}

void ShutdownProfiler()
{

}
