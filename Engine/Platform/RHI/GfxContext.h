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
DECLARE_EVENT(PostProcessVulkanExtensionsEvent, void, core::Array<core::String>&)

inline PostProcessVulkanExtensionsEvent Event_PostProcessVulkanExtensions;


enum class ENUM() GraphicsAPI
{
    Vulkan,
    D3D12,
    OpenGL,
    Null,
    Count,
};

enum class ENUM() WindowLib
{
    GLFW,
    SDL2,
};

class GfxContext
{
public:
    virtual ~GfxContext() = default;

    virtual void Initialize() = 0;
    virtual void Deinitialize() = 0;

    [[nodiscard]] virtual GraphicsAPI GetAPI() const = 0;

    [[nodiscard]] uint8_t GetSwapchainImageCount() const;
};

GfxContext& GetGfxContext();
void        UseGraphicsAPI(GraphicsAPI api);

} // namespace platform::rhi
