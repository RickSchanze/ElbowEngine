//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/Image.hpp"
#include "vulkan/vulkan.h"

namespace NRHI {
    class Surface;

    class Image_Vulkan;
    class ImageView_Vulkan;

    StringView VulkanErrorToString(VkResult result);

    struct QueueFamilyIndices {
        Optional<UInt32> graphics_family;
        Optional<UInt32> present_family;
        Optional<UInt32> transfer_family;
        Optional<UInt32> compute_family;

        bool IsComplete() const { return graphics_family && present_family && transfer_family && compute_family; }
    };

    class GfxContext_Vulkan final : public GfxContext {
    public:
        using ThisType = GfxContext_Vulkan;

        GfxContext_Vulkan();
        GfxContext_Vulkan(const GfxContext_Vulkan&) = delete;
        virtual ~GfxContext_Vulkan() override;

        virtual GraphicsAPI GetAPI() const override;

        static Array<VkExtensionProperties> GetAvailableExtensions(VkPhysicalDevice);

        virtual SwapChainSupportInfo QuerySwapChainSupportInfo() override;
        virtual const PhysicalDeviceFeature& QueryDeviceFeature() override;
        virtual const PhysicalDeviceInfo& QueryDeviceInfo() override;

        virtual Format GetDefaultDepthStencilFormat() const override;
        virtual Format GetDefaultColorFormat() const override;

        const QueueFamilyIndices &GetCurrentQueueFamilyIndices() const;

        VkImageView CreateImageView_VK(const ImageViewDesc &desc) const;
        void DestroyImageView_VK(VkImageView view) const;

        VkBuffer CreateBuffer_VK(VkDeviceSize size, VkBufferUsageFlags usage) const;
        void DestroyBuffer_VK(VkBuffer buffer) const;

        VkDeviceMemory AllocateBufferMemory_VK(VkBuffer buffer, VkMemoryPropertyFlags properties) const;
        void FreeBufferMemory_VK(VkDeviceMemory memory) const;

        void BindBufferMemory_VK(VkBuffer buffer, VkDeviceMemory memory) const;

        void MapMemory_VK(VkDeviceMemory memory, VkDeviceSize size, void **data, VkDeviceSize offset = 0) const;
        void UnmapMemory_VK(VkDeviceMemory memory) const;

        VkCommandPool CreateCommandPool_VK(const VkCommandPoolCreateInfo &info) const;
        void DestroyCommandPool_VK(VkCommandPool pool) const;
        void CreateCommandBuffers_VK(const VkCommandBufferAllocateInfo& alloc_info, VkCommandBuffer* command_buffers) const;

        virtual UniquePtr<Fence> CreateFence(bool signaled) override;

        virtual SharedPtr<LowShader> CreateShader(const char* code, size_t size, StringView debug_name) override;

        virtual NExec::ExecFuture<> Submit(SharedPtr<CommandBuffer> buffer, const SubmitParameter& parameter) override;

        virtual SharedPtr<Buffer> CreateBuffer(const BufferDesc& create_info, StringView debug_name) override;

        virtual SharedPtr<CommandPool> CreateCommandPool(const CommandPoolCreateInfo& create_info) override;

        virtual CommandPool &GetTransferPool() override { return *transfer_pool_; }

        void SetObjectDebugName(VkObjectType type, void *handle, StringView name) const;

        uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const;

        void BeginDebugLabel(VkCommandBuffer cmd, const VkDebugUtilsLabelEXT &info) const;
        void EndDebugLabel(VkCommandBuffer cmd) const;

        VkDevice GetDevice() const { return device_; }

        VkQueue GetQueue(QueueFamilyType type) const;

        virtual SharedPtr<Image> CreateImage(const ImageDesc& desc, StringView debug_name) override;

        virtual SharedPtr<ImageView> CreateImageView(const ImageViewDesc &desc, StringView debug_name) override;

        static Format GetSwapchainImageFormat();

        virtual UniquePtr<Pipeline> CreateComputePipeline(const ComputePipelineDesc &create_info) override;

        VkPhysicalDevice GetPhysicalDevice() const { return physical_device_; }

    private:
        Format FindSupportedFormat(const Array<Format> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

        void FindVulkanExtensionSymbols();

        UInt64 post_vulkan_gfx_context_init_;
        static void PostVulkanGfxContextInit(GfxContext *ctx);
        UInt64 pre_vulkan_gfx_context_destroyed_;
        static void PreVulkanGfxContextDestroyed(GfxContext *ctx);

    public:
        virtual UniquePtr<Pipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& create_info, NRHI::RenderPass* render_pass) override;

        virtual Optional<int32_t> GetCurrentSwapChainImageIndexSync(Semaphore* signal_semaphore) override;

        virtual SharedPtr<DescriptorSetLayout> CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc) override;

        virtual UniquePtr<Semaphore> Create_Semaphore(UInt64 init_value, bool timeline) override;

        virtual bool Present(uint32_t image_index, Semaphore *wait_semaphore) override;

        VkSwapchainKHR GetSwapchain() const
        {
            return swapchain_;
        }

        virtual ImageView* GetSwapChainView(UInt32 index) override;

        virtual Image* GetSwapChainImage(UInt32 index) override;

        virtual void WaitForDeviceIdle() override;

        virtual void WaitForQueueExecution(QueueFamilyType type) override;

        virtual void ResizeSwapChain(Int32 width, Int32 height) override;

        virtual SharedPtr<DescriptorSetPool> CreateDescriptorSetPool(const DescriptorSetPoolDesc& desc) override;

        virtual void BeginImGuiFrame(NRHI::CommandBuffer& cmd, Int32, Int32 w, Int32 h) override;
        virtual void EndImGuiFrame(NRHI::CommandBuffer &buffer) override;

    protected:
        virtual SharedPtr<Sampler> CreateSampler(const SamplerDesc &desc, StringView debug_name) override;

    private:
        VkInstance instance_ = nullptr;
        Surface *surface_ = nullptr;
        VkPhysicalDevice physical_device_ = nullptr;
        VkDevice device_ = nullptr;
        VkQueue graphics_queue_ = nullptr;
        VkQueue present_queue_ = nullptr;
        VkQueue transfer_queue_ = nullptr;
        VkQueue compute_queue_ = nullptr;
        VkSwapchainKHR swapchain_ = nullptr;
        QueueFamilyIndices queue_family_indices_{};

        SharedPtr<CommandPool> transfer_pool_;

        Array<Image_Vulkan *> swapchain_images_;
        Array<ImageView_Vulkan *> swapchain_image_views_;
        int32_t current_image_index_ = 0;

        Optional<PhysicalDeviceFeature> device_feature_;
        Optional<PhysicalDeviceInfo> device_info_;

        Format default_depth_stencil_format_ = Format::Count;
        Format default_color_format_ = Format::Count;

        PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT = nullptr;
        PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT = nullptr;
        PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT = nullptr;

        ImageDesc swapchain_image_desc_ = ImageDesc::Default();

#if USE_IMGUI
        VkDescriptorPool imgui_pool_ = nullptr;
        VkRenderPass imgui_render_pass_ = nullptr;
        Array<VkFramebuffer> imgui_framebuffers_ = {};
#endif
    };

    GfxContext_Vulkan *GetVulkanGfxContext();

    inline StringView VulkanErrorToString(VkResult result) {
        switch (result) {
            case VK_SUCCESS:
                return "VK_SUCCESS";
            case VK_NOT_READY:
                return "VK_NOT_READY";
            case VK_TIMEOUT:
                return "VK_TIMEOUT";
            case VK_EVENT_SET:
                return "VK_EVENT_SET";
            case VK_EVENT_RESET:
                return "VK_EVENT_RESET";
            case VK_INCOMPLETE:
                return "VK_INCOMPLETE";
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return "VK_ERROR_OUT_OF_HOST_MEMORY";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            case VK_ERROR_INITIALIZATION_FAILED:
                return "VK_ERROR_INITIALIZATION_FAILED";
            case VK_ERROR_DEVICE_LOST:
                return "VK_ERROR_DEVICE_LOST";
            case VK_ERROR_MEMORY_MAP_FAILED:
                return "VK_ERROR_MEMORY_MAP_FAILED";
            case VK_ERROR_LAYER_NOT_PRESENT:
                return "VK_ERROR_LAYER_NOT_PRESENT";
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                return "VK_ERROR_EXTENSION_NOT_PRESENT";
            case VK_ERROR_FEATURE_NOT_PRESENT:
                return "VK_ERROR_FEATURE_NOT_PRESENT";
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                return "VK_ERROR_INCOMPATIBLE_DRIVER";
            case VK_ERROR_TOO_MANY_OBJECTS:
                return "VK_ERROR_TOO_MANY_OBJECTS";
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                return "VK_ERROR_FORMAT_NOT_SUPPORTED";
            case VK_ERROR_FRAGMENTED_POOL:
                return "VK_ERROR_FRAGMENTED_POOL";
            case VK_ERROR_OUT_OF_POOL_MEMORY:
                return "VK_ERROR_OUT_OF_POOL_MEMORY";
            case VK_ERROR_INVALID_EXTERNAL_HANDLE:
                return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
            case VK_ERROR_SURFACE_LOST_KHR:
                return "VK_ERROR_SURFACE_LOST_KHR";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
            case VK_SUBOPTIMAL_KHR:
                return "VK_SUBOPTIMAL_KHR";
            case VK_ERROR_OUT_OF_DATE_KHR:
                return "VK_ERROR_OUT_OF_DATE_KHR";
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
            case VK_ERROR_VALIDATION_FAILED_EXT:
                return "VK_ERROR_VALIDATION_FAILED_EXT";
            default:
                return "Unknown error";
        }
    }

    inline void VerifyVulkanResult(VkResult result) {
        if (result != VK_SUCCESS) {
            Log(Error) << "Vulkan error: "_es + VulkanErrorToString(result);
        }
    }

} // namespace rhi
