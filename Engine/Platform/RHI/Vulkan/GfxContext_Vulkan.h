/**
 * @file GfxContext_Vulkan.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Platform/RHI/GfxContext.h"
#include "vulkan/vulkan.h"

static core::StringView VulkanErrorToString(VkResult result);

#define VERIFY_VULKAN_RESULT(result) \
    Assert(logcat::Platform_RHI_Vulkan, (result) == VK_SUCCESS, "Vulkan error: {}", VulkanErrorToString(result))

namespace platform::rhi::vulkan
{

struct QueueFamilyIndices
{
    core::Optional<uint32_t> graphics_family;
    core::Optional<uint32_t> present_family;

    bool IsComplete() const
    {
        return graphics_family.has_value() && present_family.has_value();
    }
};

class GfxContext_Vulkan final : public GfxContext
{
public:
    GfxContext_Vulkan();
    ~GfxContext_Vulkan() override;

    [[nodiscard]] GraphicsAPI GetAPI() const override;

    static core::Array<VkExtensionProperties> GetAvailableExtensions(VkPhysicalDevice);

private:
    VkInstance       instance_        = nullptr;
    VkPhysicalDevice physical_device_ = nullptr;
    VkDevice         device_          = nullptr;
};
}
