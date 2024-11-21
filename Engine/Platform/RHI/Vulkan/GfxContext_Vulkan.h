/**
 * @file GfxContext_Vulkan.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Platform/RHI/GfxContext.h"
#include "vulkan/vulkan.h"

#define VERIFY_VULKAN_RESULT(result) \
    if (result != VK_SUCCESS) { \
        throw std::runtime_error("Vulkan error: " + std::to_string(result)); \
    }

namespace platform::rhi::vulkan
{
class GfxContext_Vulkan final : public GfxContext
{
public:
    GfxContext_Vulkan();
    ~GfxContext_Vulkan() override;

    [[nodiscard]] GraphicsAPI GetAPI() const override;

private:
    VkInstance instance_ = nullptr;
};
}
