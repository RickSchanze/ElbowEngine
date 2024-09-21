/**
 * @file GfxContext.cpp
 * @author Echo 
 * @Date 24-9-21
 * @brief 
 */

#include "GfxContext.h"

#include "CoreGlobal.h"

static RHI::GfxContext* g_context;

RHI::GfxContext& RHI::GetGfxContext()
{
    ASSERT_CATEGORY(RHI, g_context != nullptr, L"GfxContext为空");
    return *g_context;
}

void RHI::SetGfxContext(GfxContext* context)
{
    ASSERT_CATEGORY(RHI, context != nullptr, L"传入GfxContext为空");
    g_context = context;
}

void RHI::ClearGfxContext()
{
    g_context = nullptr;
}
