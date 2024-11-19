/**
 * @file GfxContext_Vulkan.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Platform/RHI/GfxContext.h"
#include "vulkan/vulkan.h"

namespace platform::rhi::vulkan
{
class GfxContext_Vulkan : public GfxContext {
public:
    GfxContext_Vulkan();

    [[nodiscard]] GraphicsAPI GetAPI() const override;

private:
    VkInstance instance_ = nullptr;
};
}
