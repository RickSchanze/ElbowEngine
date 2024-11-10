/**
 * @file GfxContext.cpp
 * @author Echo 
 * @Date 24-9-21
 * @brief 
 */

#include "GfxContext.h"
#include "CoreGlobal.h"
#include "CoreDef.h"
#include "PlatformLogcat.h"

static rhi::GfxContext* g_context;

namespace rhi
{
GfxContext& GetGfxContext()
{
    Assert(logcat::Platform_RHI, g_context != nullptr, "GfxContext is null");
    return *g_context;
}

void SetGfxContext(GfxContext* context)
{
    // Assert(RHI, context != nullptr, L"传入GfxContext为空");
    g_context = context;
}

void ClearGfxContext()
{
    g_context = nullptr;
}
}
