/**
 * @file GfxContext_Vulkan.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Platform/RHI/GfxContext.h"
#include "vulkan/vulkan.h"

namespace platform::rhi
{
class Surface;
}
static core::StringView VulkanErrorToString(VkResult result);

#define VERIFY_VULKAN_RESULT(result) Assert(logcat::Platform_RHI_Vulkan, (result) == VK_SUCCESS, "Vulkan error: {}", VulkanErrorToString(result))

namespace platform::rhi::vulkan
{

struct QueueFamilyIndices
{
    core::Optional<uint32_t> graphics_family;
    core::Optional<uint32_t> present_family;

    [[nodiscard]] bool IsComplete() const { return graphics_family.has_value() && present_family.has_value(); }
};

class GfxContext_Vulkan final : public GfxContext
{
public:
    GfxContext_Vulkan();
    ~GfxContext_Vulkan() override;

    [[nodiscard]] GraphicsAPI GetAPI() const override;

    static core::Array<VkExtensionProperties> GetAvailableExtensions(VkPhysicalDevice);

    [[nodiscard]] SwapChainSupportInfo  QuerySwapChainSupportInfo() override;
    [[nodiscard]] PhysicalDeviceFeature QueryDeviceFeature() override;
    [[nodiscard]] PhysicalDeviceInfo    QueryDeviceInfo() override;

private:
    VkInstance       instance_        = nullptr;
    Surface*         surface_         = nullptr;
    VkPhysicalDevice physical_device_ = nullptr;
    VkDevice         device_          = nullptr;

    core::Optional<SwapChainSupportInfo>  swap_chain_support_info_;
    core::Optional<PhysicalDeviceFeature> device_feature_;
    core::Optional<PhysicalDeviceInfo>    device_info_;
};
}   // namespace platform::rhi::vulkan
