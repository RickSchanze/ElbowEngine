/**
 * @file GfxContext.cpp
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#include "GfxContext.h"

#include "Core/Config/ConfigManager.h"
#include "Core/CoreGlobal.h"
#include "Core/Memory/MemoryManager.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/PlatformLogcat.h"
#include "Vulkan/GfxContext_Vulkan.h"

namespace platform::rhi
{
static GfxContext* ctx;

GfxContext::GfxContext()
{
    core::MemoryManager::RequestPool(MEMORY_POOL_ID_CMD);
}

uint8_t GfxContext::GetSwapchainImageCount() const
{
    const auto cfg = core::GetConfig<PlatformConfig>();
    return cfg->GetSwapchainImageCount();
}

GfxContext* GetGfxContext()
{
    core::Assert::Require(logcat::Platform_RHI, ctx, "GfxContext not initialized");
    return ctx;
}

GfxContext& GetGfxContextRef()
{
    return *GetGfxContext();
}

void UseGraphicsAPI(GraphicsAPI api)
{
    Event_GfxContextPreInitialized.Invoke();
    switch (api)
    {
    case GraphicsAPI::Vulkan: ctx = New<vulkan::GfxContext_Vulkan>(); break;
    default: core::Assert::Require(logcat::Platform_RHI, false, "Unsupported Graphics API");
    }
    Event_GfxContextPostInitialized.Invoke(ctx);
}

void ReleaseGfxContext()
{
    Event_GfxContextPreDestroyed.Invoke(ctx);
    Delete(ctx);
    ctx = nullptr;
    Event_GfxContextPostDestroyed.Invoke();
}
}   // namespace platform::rhi
