/**
 * @file GfxContext.cpp
 * @author Echo 
 * @Date 24-9-21
 * @brief 
 */

#include "CoreGlobal.h"
#include "GfxContext.h"

static rhi::GfxContext* g_context;

namespace rhi
{
GfxContext& GetGfxContext()
{
    Assert(RHI, g_context != nullptr, L"GfxContext为空");
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
