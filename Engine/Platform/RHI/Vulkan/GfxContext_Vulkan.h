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
class CommandPool_Vulkan;
class ImageView_Vulkan;
}   // namespace platform::rhi::vulkan
namespace platform::rhi::vulkan
{
class Image_Vulkan;
}
namespace platform::rhi
{
struct ImageViewDesc;
class Surface;
}   // namespace platform::rhi

inline core::StringView VulkanErrorToString(VkResult result);

#define VERIFY_VULKAN_RESULT(result)                                                                                                 \
    {                                                                                                                                \
        auto result_ = result;                                                                                                       \
        core::Assert::Require(logcat::Platform_RHI_Vulkan, result_ == VK_SUCCESS, "Vulkan error: {}", VulkanErrorToString(result_)); \
    }

namespace platform::rhi::vulkan
{

struct QueueFamilyIndices
{
    core::Optional<uint32_t> graphics_family;
    core::Optional<uint32_t> present_family;
    core::Optional<uint32_t> transfer_family;

    [[nodiscard]] bool IsComplete() const { return graphics_family.HasValue() && present_family.HasValue() && transfer_family.HasValue(); }
};

class GfxContext_Vulkan final : public GfxContext
{
public:
    using ThisType = GfxContext_Vulkan;

    GfxContext_Vulkan();
    GfxContext_Vulkan(const GfxContext_Vulkan&) = delete;
    ~GfxContext_Vulkan() override;

    [[nodiscard]] GraphicsAPI GetAPI() const override;

    static core::Array<VkExtensionProperties> GetAvailableExtensions(VkPhysicalDevice);

    [[nodiscard]] const SwapChainSupportInfo&  QuerySwapChainSupportInfo() override;
    [[nodiscard]] const PhysicalDeviceFeature& QueryDeviceFeature() override;
    [[nodiscard]] const PhysicalDeviceInfo&    QueryDeviceInfo() override;

    [[nodiscard]] Format GetDefaultDepthStencilFormat() const override;
    [[nodiscard]] Format GetDefaultColorFormat() const override;

    [[nodiscard]] const QueueFamilyIndices& GetCurrentQueueFamilyIndices() const;

#if REGION(资源创建/销毁: Vulkan特化)
    [[nodiscard]] VkImageView CreateImageView_VK(const ImageViewDesc& desc) const;
    void                      DestroyImageView_VK(VkImageView view) const;

    [[nodiscard]] VkBuffer CreateBuffer_VK(VkDeviceSize size, VkBufferUsageFlags usage) const;
    void                   DestroyBuffer_VK(VkBuffer buffer) const;

    [[nodiscard]] VkDeviceMemory AllocateBufferMemory_VK(VkBuffer buffer, VkMemoryPropertyFlags properties) const;
    void                         FreeBufferMemory_VK(VkDeviceMemory memory) const;

    void BindBufferMemory_VK(VkBuffer buffer, VkDeviceMemory memory) const;

    void MapMemory_VK(VkDeviceMemory memory, VkDeviceSize size, void** data, VkDeviceSize offset = 0) const;
    void UnmapMemory_VK(VkDeviceMemory memory) const;

    [[nodiscard]] VkCommandPool CreateCommandPool_VK(const VkCommandPoolCreateInfo& info) const;
    void                        DestroyCommandPool_VK(VkCommandPool pool) const;
    void                        CreateCommandBuffers_VK(const VkCommandBufferAllocateInfo& alloc_info, VkCommandBuffer* command_buffers) const;

    [[nodiscard]] core::SharedPtr<Fence> CreateFence() override;

    [[nodiscard]] core::SharedPtr<LowShader> CreateShader(const char* code, size_t size, core::StringView debug_name) override;
#endif

    core::exec::AsyncResultHandle<> Submit(CommandBuffer& buffer, const SubmitParameter& parameter) override;

    [[nodiscard]] core::SharedPtr<Buffer> CreateBuffer(const BufferCreateInfo& create_info) override;

    [[nodiscard]] core::SharedPtr<CommandPool> CreateCommandPool(const CommandPoolCreateInfo& create_info) override;

    [[nodiscard]] CommandPool& GetTransferPool() override { return *transfer_pool_; }

    void SetObjectDebugName(VkObjectType type, void* handle, core::StringView name) const;

    [[nodiscard]] uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const;

    void BeginDebugLabel(VkCommandBuffer cmd, const VkDebugUtilsLabelEXT& info) const;
    void EndDebugLabel(VkCommandBuffer cmd) const;

    [[nodiscard]] VkDevice GetDevice() const { return device_; }

    [[nodiscard]] VkQueue GetQueue(QueueFamilyType type) const;

private:
    [[nodiscard]] Format FindSupportedFormat(const core::Array<Format>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

    void FindVulkanExtensionSymbols();

    core::DelegateID post_vulkan_gfx_context_init_;
    static void      PostVulkanGfxContextInit(GfxContext* ctx);
    core::DelegateID pre_vulkan_gfx_context_destroyed_;
    static void      PreVulkanGfxContextDestroyed(GfxContext* ctx);

public:
    [[nodiscard]] core::UniquePtr<GraphicsPipeline>
    CreateGraphicsPipeline(const GraphicsPipelineDesc& create_info, rhi::RenderPass* render_pass) override;

    [[nodiscard]] core::Optional<int32_t> GetCurrentSwapChainImageIndexSync(Semaphore* signal_semaphore) override;

    [[nodiscard]] core::SharedPtr<DescriptorSetLayout> CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc) override;

    [[nodiscard]] core::UniquePtr<Semaphore> CreateASemaphore(uint64_t init_value, bool timeline) override;

    bool Present(uint32_t image_index, Semaphore* wait_semaphore) override;

    [[nodiscard]] VkSwapchainKHR GetSwapchain() const { return swapchain_; }

private:
    VkInstance         instance_        = nullptr;
    Surface*           surface_         = nullptr;
    VkPhysicalDevice   physical_device_ = nullptr;
    VkDevice           device_          = nullptr;
    VkQueue            graphics_queue_  = nullptr;
    VkQueue            present_queue_   = nullptr;
    VkQueue            transfer_queue_  = nullptr;
    VkSwapchainKHR     swapchain_       = nullptr;
    QueueFamilyIndices queue_family_indices_{};

    core::SharedPtr<CommandPool> transfer_pool_;

    core::Array<Image_Vulkan*>     swapchain_images_;
    core::Array<ImageView_Vulkan*> swapchain_image_views_;
    int32_t                        current_image_index_ = 0;

    core::Optional<SwapChainSupportInfo>  swap_chain_support_info_;
    core::Optional<PhysicalDeviceFeature> device_feature_;
    core::Optional<PhysicalDeviceInfo>    device_info_;

    Format default_depth_stencil_format_ = Format::Count;
    Format default_color_format_         = Format::Count;

    PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT = nullptr;
    PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT = nullptr;
    PFN_vkCmdEndDebugUtilsLabelEXT   CmdEndDebugUtilsLabelEXT   = nullptr;

    ImageDesc swapchain_image_desc_ = ImageDesc::Default();
};

GfxContext_Vulkan* GetVulkanGfxContext();

}   // namespace platform::rhi::vulkan
