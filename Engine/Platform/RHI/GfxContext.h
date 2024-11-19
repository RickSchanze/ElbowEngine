/**
 * @file GfxContext.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include <cstdint>

namespace platform::rhi
{

enum class GraphicsAPI
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
void        UseGraphicsAPI(GraphicsAPI api);

}   // namespace platform::rhi
