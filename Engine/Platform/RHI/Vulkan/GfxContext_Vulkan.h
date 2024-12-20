/**
 * @file GfxContext_Vulkan.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/Image.h"
#include "vulkan/vulkan.h"

namespace platform::rhi::vulkan
{
class ImageView_Vulkan;
}
namespace platform::rhi::vulkan
{
class Image_Vulkan;
}
namespace platform::rhi
{
struct ImageViewDesc;
class Surface;
}   // namespace platform::rhi
static core::StringView VulkanErrorToString(VkResult result);

#define VERIFY_VULKAN_RESULT(result) \
    auto result_ = result;           \
    core::Assert::Require(logcat::Platform_RHI_Vulkan, result_ == VK_SUCCESS, "Vulkan error: {}", VulkanErrorToString(result_))

namespace platform::rhi::vulkan
{

struct QueueFamilyIndices
{
    core::Optional<uint32_t> graphics_family;
    core::Optional<uint32_t> present_family;

    [[nodiscard]] bool IsComplete() const { return graphics_family.HasValue() && present_family.HasValue(); }
};

class GfxContext_Vulkan final : public GfxContext
{
public:
    using ThisType = GfxContext_Vulkan;

    GfxContext_Vulkan();
    ~GfxContext_Vulkan() override;

    [[nodiscard]] GraphicsAPI GetAPI() const override;

    static core::Array<VkExtensionProperties> GetAvailableExtensions(VkPhysicalDevice);

    [[nodiscard]] const SwapChainSupportInfo&  QuerySwapChainSupportInfo() override;
    [[nodiscard]] const PhysicalDeviceFeature& QueryDeviceFeature() override;
    [[nodiscard]] const PhysicalDeviceInfo&    QueryDeviceInfo() override;

    [[nodiscard]] Format GetDefaultDepthStencilFormat() const override;
    [[nodiscard]] Format GetDefaultColorFormat() const override;

#if REGION(资源创建/销毁)
    [[nodiscard]] VkImageView CreateImageView(const ImageViewDesc& desc) const;
    void                      DestroyImageView(VkImageView view) const;

    [[nodiscard]] VkBuffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const;
    void                   DestroyBuffer(VkBuffer buffer) const;

    [[nodiscard]] VkDeviceMemory AllocateBufferMemory(VkBuffer buffer, VkMemoryPropertyFlags properties) const;
    void                         FreeBufferMemory(VkDeviceMemory memory) const;

    void BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory) const;
#endif

    void SetObjectDebugName(VkObjectType type, void* handle, core::StringView name) const;

    [[nodiscard]] uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const;

private:
    [[nodiscard]] Format FindSupportedFormat(const core::Array<Format>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

    void FindVulkanExtensionSymbols();

    core::DelegateID post_vulkan_gfx_context_init_;
    static void      PostVulkanGfxContextInit(GfxContext* ctx);
    core::DelegateID pre_vulkan_gfx_context_destroyed_;
    static void      PreVulkanGfxContextDestroyed(GfxContext* ctx);

    VkInstance       instance_        = nullptr;
    Surface*         surface_         = nullptr;
    VkPhysicalDevice physical_device_ = nullptr;
    VkDevice         device_          = nullptr;
    VkQueue          graphics_queue_  = nullptr;
    VkQueue          present_queue_   = nullptr;
    VkSwapchainKHR   swapchain_       = nullptr;

    core::Array<Image_Vulkan*>     swapchain_images_;
    core::Array<ImageView_Vulkan*> swapchain_image_views_;

    core::Optional<SwapChainSupportInfo>  swap_chain_support_info_;
    core::Optional<PhysicalDeviceFeature> device_feature_;
    core::Optional<PhysicalDeviceInfo>    device_info_;

    Format default_depth_stencil_format_ = Format::Count;
    Format default_color_format_         = Format::Count;

    PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT = nullptr;

    ImageDesc swapchain_image_desc_ = ImageDesc::Default();
};

GfxContext_Vulkan* GetVulkanGfxContext();

}   // namespace platform::rhi::vulkan
