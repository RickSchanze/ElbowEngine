/**
 * @file GfxContext.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include <cstdint>
#include "Core/Core.h"
#include "Core/Event/Event.h"


namespace platform::rhi
{
DECLARE_EVENT(PostProcessVulkanExtensionsEvent, void, core::Ref<core::Array<const char*>>)

inline PostProcessVulkanExtensionsEvent Event_PostProcessVulkanExtensions;


enum class ENUM() GraphicsAPI
{
    Vulkan,
    D3D12,
    OpenGL,
    Null,
    Count,
};

class GfxContext
{
public:
    virtual ~GfxContext() = default;

    [[nodiscard]] virtual GraphicsAPI GetAPI() const = 0;

    [[nodiscard]] uint8_t GetSwapchainImageCount() const;
};

GfxContext& GetGfxContext();

/**
 * 设置使用的图形API并以此创建GfxContext
 * @param api 图形API
 */
void UseGraphicsAPI(GraphicsAPI api);

/**
 * 释放GfxContext
 */
void ReleaseGfxContext();

} // namespace platform::rhi
