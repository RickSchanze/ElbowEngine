/**
 * @file GfxContext_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#include "GfxContext_Vulkan.h"

#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Platform/Config/RHIConfig.h"

platform::rhi::vulkan::GfxContext_Vulkan::GfxContext_Vulkan()
{
    auto* rhi_cfg = core::GetConfig<RHIConfig>();
    auto* core_cfg = core::GetConfig<core::CoreConfig>();
}

platform::rhi::GraphicsAPI platform::rhi::vulkan::GfxContext_Vulkan::GetAPI() const
{
    return GraphicsAPI::Vulkan;
}
