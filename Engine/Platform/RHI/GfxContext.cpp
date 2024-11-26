/**
 * @file GfxContext.cpp
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#include "GfxContext.h"

#include "Core/Config/ConfigManager.h"
#include "Core/CoreGlobal.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/PlatformLogcat.h"
#include "Vulkan/GfxContext_Vulkan.h"

namespace platform::rhi
{
static GfxContext* ctx;

uint8_t GfxContext::GetSwapchainImageCount() const
{
    const auto cfg = core::GetConfig<PlatformConfig>();
    return cfg->GetSwapchainImageCount();
}

GfxContext* GetGfxContext()
{
    Assert(logcat::Platform_RHI, ctx, "GfxContext not initialized");
    return ctx;
}

GfxContext& GetGfxContextRef()
{
    return *GetGfxContext();
}

void UseGraphicsAPI(GraphicsAPI api)
{
    switch (api)
    {
    case GraphicsAPI::Vulkan: ctx = New<vulkan::GfxContext_Vulkan>(); break;
    default: Assert(logcat::Platform_RHI, false, "Unsupported Graphics API");
    }
}

void ReleaseGfxContext()
{
    Delete(ctx);
}
}   // namespace platform::rhi
