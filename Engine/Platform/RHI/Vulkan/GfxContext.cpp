//
// Created by Echo on 2025/3/22.
//

#include "GfxContext.hpp"

#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "Core/Async/Exec/Just.hpp"
#include "Core/Async/Exec/Then.hpp"
#include "Core/Async/ThreadManager.hpp"
#include "Core/Config/ConfigManager.hpp"
#include "Core/Config/CoreConfig.hpp"
#include "Core/Profile.hpp"
#include "DescriptorSet.hpp"
#include "Enums.hpp"
#include "Image.hpp"
#include "ImageView.hpp"
#include "LowShader.hpp"
#include "Pipeline.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/Events.hpp"
#include "Platform/RHI/ImageView.hpp"
#include "Platform/RHI/Surface.hpp"
#include "Platform/RHI/SyncPrimitives.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "SyncPrimitives.hpp"
#include "imgui.h"
#include "imgui_impl_vulkan.h"

using namespace exec;

static Array<VkLayerProperties> available_layers;
static Array<VkPhysicalDevice> physical_devices;
static Array<String> required_instance_extensions;

static Array<VkPhysicalDevice> GetAvailablePhysicalDevices(VkInstance instance)
{
    uint32_t device_cnt = 0;
    vkEnumeratePhysicalDevices(instance, &device_cnt, nullptr);
    Assert(device_cnt > 0, "No physical device found");
    Array<VkPhysicalDevice> physical_devices;
    physical_devices.Resize(device_cnt);
    vkEnumeratePhysicalDevices(instance, &device_cnt, physical_devices.Data());
    return physical_devices;
}

static Array<VkLayerProperties> GetAvailableLayers()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    Array<VkLayerProperties> available_layers;
    available_layers.Resize(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.Data());
    return available_layers;
}

static bool SupportValidationLayer(StringView layer_name)
{
    for (auto& layer : available_layers)
    {
        if (layer_name == layer.layerName)
        {
            return true;
        }
    }
    return false;
}

using namespace NRHI;
GraphicsAPI GfxContext_Vulkan::GetAPI() const
{
    return GraphicsAPI::Vulkan;
}

Array<VkExtensionProperties> GfxContext_Vulkan::GetAvailableExtensions(VkPhysicalDevice)
{
    uint32_t extension_count;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    Array<VkExtensionProperties> available_extensions;
    available_extensions.Resize(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.Data());
    return available_extensions;
}

static Vector2f FromVkExtent2D(const VkExtent2D& extent)
{
    Vector2f size{};
    size.X = extent.width;
    size.Y = extent.height;
    return size;
}

static SwapChainSupportInfo QuerySwapChainSupportInfo(const VkPhysicalDevice device, const Surface* surface)
{
    SwapChainSupportInfo info;
    const auto surfaceVk = static_cast<VkSurfaceKHR>(surface->GetNativeHandle());
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surfaceVk, &capabilities);
    info.current_extent = FromVkExtent2D(capabilities.currentExtent);
    info.min_image_extent = FromVkExtent2D(capabilities.minImageExtent);
    info.max_image_extent = FromVkExtent2D(capabilities.maxImageExtent);
    info.min_image_count = capabilities.minImageCount;
    info.max_image_count = capabilities.maxImageCount;
    info.max_image_array_layers = capabilities.maxImageArrayLayers;

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceVk, &format_count, nullptr);
    if (format_count != 0)
    {
        Array<VkSurfaceFormatKHR> formats;
        formats.Resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceVk, &format_count, formats.Data());
        info.formats =
            formats | NRange::NView::Select([](const VkSurfaceFormatKHR& format) {
                return SurfaceFormat{.format = VkFormatToRHIFormat(format.format), .color_space = VkColorSpaceToRHIColorSpace(format.colorSpace)};
            }) |
            NRange::To<Array<SurfaceFormat>>();
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceVk, &present_mode_count, nullptr);
    if (present_mode_count != 0)
    {
        Array<VkPresentModeKHR> present_modes;
        present_modes.Resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceVk, &present_mode_count, present_modes.Data());
        info.present_modes = present_modes | NRange::NView::Select([](VkPresentModeKHR mode) { return VkPresentModeToRHIPresentMode(mode); }) |
                             NRange::To<Array<PresentMode>>();
    }
    return info;
}

SwapChainSupportInfo GfxContext_Vulkan::QuerySwapChainSupportInfo()
{
    return ::QuerySwapChainSupportInfo(physical_device_, surface_);
}

const PhysicalDeviceFeature& GfxContext_Vulkan::QueryDeviceFeature()
{
    if (device_feature_)
        return *device_feature_;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physical_device_, &features);
    PhysicalDeviceFeature feature;
    feature.sampler_anisotropy = features.samplerAnisotropy;
    device_feature_ = feature;
    return *device_feature_;
}

const PhysicalDeviceInfo& GfxContext_Vulkan::QueryDeviceInfo()
{
    if (device_info_)
        return *device_info_;
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device_, &properties);
    PhysicalDeviceInfo info;
    info.name = properties.deviceName;
    info.limits.framebuffer_color_sample_count = static_cast<int32_t>(properties.limits.framebufferColorSampleCounts);
    info.limits.framebuffer_depth_sample_count = static_cast<int32_t>(properties.limits.framebufferDepthSampleCounts);
    device_info_ = info;
    return *device_info_;
}

Format GfxContext_Vulkan::GetDefaultDepthStencilFormat() const
{
    return default_depth_stencil_format_;
}

Format GfxContext_Vulkan::GetDefaultColorFormat() const
{
    // TODO: 灵活切换是否HDR渲染
    return Format::R32G32B32A32_Float;
}

const QueueFamilyIndices& GfxContext_Vulkan::GetCurrentQueueFamilyIndices() const
{
    return queue_family_indices_;
}

VkImageView GfxContext_Vulkan::CreateImageView_VK(const ImageViewDesc& desc) const
{
    const auto img = desc.image;
    const auto img_handle = static_cast<VkImage>(img->GetNativeHandle());
    Assert(img, "Image cannot be nullptr when creating a ImageView.");
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = img_handle;
    view_info.components = FromComponentMapping(desc.component_mapping);
    view_info.format = RHIFormatToVkFormat(desc.format);
    view_info.subresourceRange.aspectMask = RHIImageAspectToVkImageAspect(desc.subresource_range.aspect_mask);
    view_info.subresourceRange.baseMipLevel = desc.subresource_range.base_mip_level;
    view_info.subresourceRange.levelCount = desc.subresource_range.level_count;
    view_info.subresourceRange.baseArrayLayer = desc.subresource_range.base_array_layer;
    view_info.subresourceRange.layerCount = desc.subresource_range.layer_count;
    view_info.viewType = RHIImageDimensionToVkImageViewType(desc.type);
    view_info.pNext = nullptr;
    VkImageView view_handle = VK_NULL_HANDLE;
    if (const VkResult result = vkCreateImageView(device_, &view_info, nullptr, &view_handle); result != VK_SUCCESS)
    {
        Log(Error) << "Failed to create ImageView: "_es + NRHI::VulkanErrorToString(result);
    }
    return view_handle;
}

void GfxContext_Vulkan::DestroyImageView_VK(const VkImageView view) const
{
    vkDestroyImageView(device_, view, nullptr);
}

VkBuffer GfxContext_Vulkan::CreateBuffer_VK(VkDeviceSize size, VkBufferUsageFlags usage) const
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkBuffer buffer;
    const VkResult result = vkCreateBuffer(device_, &buffer_info, nullptr, &buffer);
    VerifyVulkanResult(result);
    return buffer;
}

void GfxContext_Vulkan::DestroyBuffer_VK(VkBuffer buffer) const
{
    vkDestroyBuffer(device_, buffer, nullptr);
}

VkDeviceMemory GfxContext_Vulkan::AllocateBufferMemory_VK(VkBuffer buffer, VkMemoryPropertyFlags properties) const
{
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device_, buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);
    VkDeviceMemory memory;
    const VkResult result = vkAllocateMemory(device_, &alloc_info, nullptr, &memory);
    VerifyVulkanResult(result);
    return memory;
}

void GfxContext_Vulkan::FreeBufferMemory_VK(VkDeviceMemory memory) const
{
    vkFreeMemory(device_, memory, nullptr);
}

void GfxContext_Vulkan::BindBufferMemory_VK(VkBuffer buffer, VkDeviceMemory memory) const
{
    vkBindBufferMemory(device_, buffer, memory, 0);
}

void GfxContext_Vulkan::MapMemory_VK(VkDeviceMemory memory, VkDeviceSize size, void** data, VkDeviceSize offset) const
{
    vkMapMemory(device_, memory, offset, size, 0, data);
}

void GfxContext_Vulkan::UnmapMemory_VK(VkDeviceMemory memory) const
{
    vkUnmapMemory(device_, memory);
}

VkCommandPool GfxContext_Vulkan::CreateCommandPool_VK(const VkCommandPoolCreateInfo& info) const
{
    VkCommandPool command_pool_;
    const auto result = vkCreateCommandPool(device_, &info, nullptr, &command_pool_);
    VerifyVulkanResult(result);
    return command_pool_;
}

void GfxContext_Vulkan::DestroyCommandPool_VK(VkCommandPool pool) const
{
    vkDestroyCommandPool(device_, pool, nullptr);
}

void GfxContext_Vulkan::CreateCommandBuffers_VK(const VkCommandBufferAllocateInfo& alloc_info, VkCommandBuffer* command_buffers) const
{
    auto* cfg = GetConfig<PlatformConfig>();
    auto task = Just() | Then([alloc_info, command_buffers, this] { vkAllocateCommandBuffers(device_, &alloc_info, command_buffers); });
    if (cfg->GetEnableMultithreadRender())
    {

        ThreadManager::ScheduleFutureAsync(task, NamedThread::Render).Wait();
    }
    else
    {
        ThreadManager::ScheduleFutureAsync(task, NamedThread::Game, true).Wait();
    }
}

UniquePtr<Fence> GfxContext_Vulkan::CreateFence(bool signaled)
{
    return MakeUnique<Fence_Vulkan>(signaled);
}

SharedPtr<LowShader> GfxContext_Vulkan::CreateShader(const char* code, size_t size, StringView debug_name)
{
    auto rtn = MakeShared<LowShader_Vulkan>(code, size);
#ifdef ELBOW_DEBUG
    SetObjectDebugName(VK_OBJECT_TYPE_SHADER_MODULE, rtn->GetNativeHandleT<VkShaderModule>(), debug_name);
#endif
    return rtn;
}

static void InternalSubmit(const SharedPtr<CommandBuffer>& buffer, const SubmitParameter& parameter)
{
    auto* ctx = GetVulkanGfxContext();
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    VkCommandBuffer command_buffer = buffer->GetNativeHandleT<VkCommandBuffer>();
    submit_info.pCommandBuffers = &command_buffer;
    VkTimelineSemaphoreSubmitInfo timeline_submit_info{};
    timeline_submit_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    VkSemaphore signal_semaphore = VK_NULL_HANDLE;
    VkSemaphore wait_semaphore = VK_NULL_HANDLE;
    if (parameter.signal_semaphore != nullptr)
    {
        if (parameter.signal_semaphore->Vulkan_IsTimelineSemaphore())
        {
            timeline_submit_info.signalSemaphoreValueCount = 1;
            timeline_submit_info.pSignalSemaphoreValues = &parameter.signal_value;
        }
        submit_info.signalSemaphoreCount = 1;
        signal_semaphore = parameter.signal_semaphore->GetNativeHandleT<VkSemaphore>();
        submit_info.pSignalSemaphores = &signal_semaphore;
    }
    if (parameter.wait_semaphore != nullptr)
    {
        if (parameter.wait_semaphore->Vulkan_IsTimelineSemaphore())
        {
            timeline_submit_info.waitSemaphoreValueCount = 1;
            timeline_submit_info.pWaitSemaphoreValues = &parameter.wait_value;
        }
        submit_info.waitSemaphoreCount = 1;
        wait_semaphore = parameter.wait_semaphore->GetNativeHandleT<VkSemaphore>();
        submit_info.pWaitSemaphores = &wait_semaphore;
    }
    submit_info.pNext = &timeline_submit_info;
    // TODO: wait_stages参数化
    VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit_info.pWaitDstStageMask = &wait_stages;
    VkFence fence = VK_NULL_HANDLE;
    if (parameter.fence != nullptr)
    {
        fence = parameter.fence->GetNativeHandleT<VkFence>();
    }
    vkQueueSubmit(ctx->GetQueue(parameter.submit_queue_type), 1, &submit_info, fence);
}

ExecFuture<> GfxContext_Vulkan::Submit(SharedPtr<CommandBuffer> buffer, const SubmitParameter& parameter)
{
    auto* cfg = GetConfig<PlatformConfig>();
    auto task = Just() | Then([buffer, &parameter] { InternalSubmit(buffer, parameter); });
    if (cfg->GetEnableMultithreadRender())
    {
        return ThreadManager::ScheduleFutureAsync(task, NamedThread::Render);
    }
    else
    {
        return ThreadManager::ScheduleFutureAsync(task, NamedThread::Game, true);
    }
}

SharedPtr<Buffer> GfxContext_Vulkan::CreateBuffer(const BufferDesc& create_info, StringView name)
{
    auto rtn = MakeShared<Buffer_Vulkan>(create_info);
#ifdef ELBOW_DEBUG
    if (!name.IsEmpty())
    {
        SetObjectDebugName(VK_OBJECT_TYPE_BUFFER, rtn->GetNativeHandle(), *name);
    }
#endif
    return rtn;
}

SharedPtr<CommandPool> GfxContext_Vulkan::CreateCommandPool(const CommandPoolCreateInfo& create_info)
{
    return MakeShared<CommandPool_Vulkan>(create_info);
}

void GfxContext_Vulkan::SetObjectDebugName(const VkObjectType type, void* handle, const StringView name) const
{
#ifdef ELBOW_DEBUG
    if (GetConfig<PlatformConfig>()->GetValidEnableValidationLayer() == false)
        return;
    VkDebugUtilsObjectNameInfoEXT name_info = {};
    name_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    name_info.objectType = type;
    name_info.objectHandle = reinterpret_cast<uint64_t>(handle);
    name_info.pObjectName = name.Data();
    if (const VkResult result = SetDebugUtilsObjectNameEXT(device_, &name_info); result != VK_SUCCESS)
    {
        Log(Fatal) << "Failed to set debug name for object "_es + VulkanErrorToString(result);
    }
#endif
}

UInt32 GfxContext_Vulkan::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
    {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    Log(Fatal) << "未找到合适的内存类型";
    return 0;
}

void GfxContext_Vulkan::BeginDebugLabel(VkCommandBuffer cmd, const VkDebugUtilsLabelEXT& info) const
{
    if (GetConfig<PlatformConfig>()->GetValidEnableValidationLayer() == false)
        return;
    CmdBeginDebugUtilsLabelEXT(cmd, &info);
}

void GfxContext_Vulkan::EndDebugLabel(VkCommandBuffer cmd) const
{
    if (GetConfig<PlatformConfig>()->GetValidEnableValidationLayer() == false)
        return;
    CmdEndDebugUtilsLabelEXT(cmd);
}

VkQueue GfxContext_Vulkan::GetQueue(QueueFamilyType type) const
{
    switch (type)
    {
    case QueueFamilyType::Graphics:
        return graphics_queue_;
        break;
    case QueueFamilyType::Compute:
        return compute_queue_;
        break;
    case QueueFamilyType::Transfer:
        return transfer_queue_;
        break;
    case QueueFamilyType::Present:
        return present_queue_;
    }
    return VK_NULL_HANDLE;
}

SharedPtr<Image> GfxContext_Vulkan::CreateImage(const ImageDesc& desc, StringView debug_name)
{
    auto rtn = MakeShared<Image_Vulkan>(desc);
#ifdef ELBOW_DEBUG
    if (!debug_name.IsEmpty())
    {
        SetObjectDebugName(VK_OBJECT_TYPE_IMAGE, rtn->GetNativeHandle(), debug_name);
    }
#endif
    return rtn;
}

SharedPtr<ImageView> GfxContext_Vulkan::CreateImageView(const ImageViewDesc& desc, StringView debug_name)
{
    auto rtn = MakeShared<ImageView_Vulkan>(desc);
#ifdef ELBOW_DEBUG
    if (!debug_name.IsEmpty())
    {
        SetObjectDebugName(VK_OBJECT_TYPE_IMAGE_VIEW, rtn->GetNativeHandle(), debug_name);
    }
#endif
    return rtn;
}

Format GfxContext_Vulkan::GetSwapchainImageFormat()
{
    return Format::B8G8R8A8_UNorm;
}

UniquePtr<Pipeline> GfxContext_Vulkan::CreateComputePipeline(const ComputePipelineDesc& create_info)
{
    return MakeUnique<ComputePipeline_Vulkan>(create_info);
}

Format GfxContext_Vulkan::FindSupportedFormat(const Array<Format>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
    for (const Array<VkFormat> fmts =
             candidates | NRange::NView::Select([](Format fmt) { return RHIFormatToVkFormat(fmt); }) | NRange::To<Array<VkFormat>>();
         const auto fmt : fmts)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physical_device_, fmt, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return VkFormatToRHIFormat(fmt);
        }
        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return VkFormatToRHIFormat(fmt);
        }
    }
    return Format::Count;
}

void GfxContext_Vulkan::FindVulkanExtensionSymbols()
{
    SetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance_, "vkSetDebugUtilsObjectNameEXT"));
    CmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetDeviceProcAddr(device_, "vkCmdBeginDebugUtilsLabelEXT"));
    CmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetDeviceProcAddr(device_, "vkCmdEndDebugUtilsLabelEXT"));
}

constexpr const char* swapchain_image_view_names_[] = {
    "SwapChainImageView0", "SwapChainImageView1", "SwapChainImageView2", "SwapChainImageView3",
    "SwapChainImageView4", "SwapChainImageView5", "SwapChainImageView6", "SwapChainImageView7",
    "SwapChainImageView8", // 开发设备最多支持八个
};

void GfxContext_Vulkan::PostVulkanGfxContextInit(GfxContext* ctx)
{
    auto vulkan_ctx = static_cast<GfxContext_Vulkan*>(ctx);
    UInt32 img_cnt = 0;
    VerifyVulkanResult(vkGetSwapchainImagesKHR(vulkan_ctx->device_, vulkan_ctx->swapchain_, &img_cnt, nullptr));
    auto vk_imgs = Array<VkImage>{};
    vk_imgs.Resize(img_cnt);
    VerifyVulkanResult(vkGetSwapchainImagesKHR(vulkan_ctx->device_, vulkan_ctx->swapchain_, &img_cnt, vk_imgs.Data()));
    if (img_cnt <= 0)
    {
        Log(Fatal) << "创建交换链失败";
    }
    vulkan_ctx->swapchain_images_ = //
        vk_imgs | NRange::NView::Enumerate | NRange::NView::Select([vulkan_ctx](const auto& pair) {
            auto& desc = vulkan_ctx->swapchain_image_desc_;
            const auto& [idx, img] = pair;
            return New<Image_Vulkan>(img, idx, desc.Width, desc.Height, desc.Format);
        }) |
        NRange::To<Array<Image_Vulkan*>>();
    vulkan_ctx->swapchain_image_views_ = //
        vulkan_ctx->swapchain_images_ | NRange::NView::Enumerate | NRange::NView::Select([](const auto& pair) {
            const auto& [idx, img] = pair;
            ImageViewDesc desc(img);
            return New<ImageView_Vulkan>(desc);
        }) |
        NRange::To<Array<ImageView_Vulkan*>>();

    CommandPoolCreateInfo transfer_pool_info{QueueFamilyType::Transfer, true};
    vulkan_ctx->transfer_pool_ = vulkan_ctx->CreateCommandPool(transfer_pool_info);

#if USE_IMGUI
    // 初始化ImGui环境
    auto cfg = GetConfig<PlatformConfig>();
    VkDescriptorPoolSize pool_size[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, cfg->GetMinImGuiImageSamplerPoolSize()},
    };
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 0;
    for (const auto& size : pool_size)
    {
        pool_info.maxSets += size.descriptorCount;
    }
    pool_info.poolSizeCount = std::size(pool_size);
    pool_info.pPoolSizes = pool_size;
    VkDescriptorPool pool;
    VerifyVulkanResult(vkCreateDescriptorPool(vulkan_ctx->device_, &pool_info, nullptr, &pool));

    VkAttachmentDescription attachment = {};
    attachment.format = RHIFormatToVkFormat(vulkan_ctx->GetSwapchainImageFormat());
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference color_attachment = {};
    color_attachment.attachment = 0;
    color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment;
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    VkRenderPass render_pass = {};
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    VerifyVulkanResult(vkCreateRenderPass(vulkan_ctx->device_, &info, nullptr, &render_pass));

    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = render_pass;
    framebuffer_create_info.width = cfg->GetDefaultWindowSize().X;
    framebuffer_create_info.height = cfg->GetDefaultWindowSize().Y;
    framebuffer_create_info.layers = 1;
    framebuffer_create_info.attachmentCount = 1;
    vulkan_ctx->imgui_framebuffers_.Resize(vulkan_ctx->swapchain_image_views_.Count());
    for (UInt32 i = 0; i < vulkan_ctx->imgui_framebuffers_.Count(); i++)
    {
        VkImageView view = vulkan_ctx->swapchain_image_views_[i]->GetNativeHandleT<VkImageView>();
        framebuffer_create_info.pAttachments = &view;
        VerifyVulkanResult(vkCreateFramebuffer(vulkan_ctx->device_, &framebuffer_create_info, nullptr, &vulkan_ctx->imgui_framebuffers_[i]));
    }

    ImGui_ImplVulkan_InitInfo imgui_info = {};
    imgui_info.Instance = vulkan_ctx->instance_;
    imgui_info.PhysicalDevice = vulkan_ctx->physical_device_;
    imgui_info.Device = vulkan_ctx->device_;
    imgui_info.Queue = vulkan_ctx->graphics_queue_;
    imgui_info.DescriptorPool = pool;
    imgui_info.MinImageCount = vulkan_ctx->swapchain_image_views_.Count();
    imgui_info.ImageCount = vulkan_ctx->swapchain_image_views_.Count();
    imgui_info.Subpass = 0;
    imgui_info.CheckVkResultFn = VerifyVulkanResult;
    imgui_info.RenderPass = render_pass;
    Assert(ImGui_ImplVulkan_Init(&imgui_info), "ImGui初始化失败!");
    vulkan_ctx->imgui_pool_ = pool;
    vulkan_ctx->imgui_render_pass_ = render_pass;
#endif
}

void GfxContext_Vulkan::PreVulkanGfxContextDestroyed(GfxContext* ctx)
{
    auto vulkan_ctx = static_cast<GfxContext_Vulkan*>(ctx);
    vkDeviceWaitIdle(vulkan_ctx->device_);
#if USE_IMGUI
    ImGui_ImplVulkan_Shutdown();
    vkDestroyRenderPass(vulkan_ctx->device_, vulkan_ctx->imgui_render_pass_, nullptr);
    for (const auto framebuffer : vulkan_ctx->imgui_framebuffers_)
    {
        vkDestroyFramebuffer(vulkan_ctx->device_, framebuffer, nullptr);
    }
    if (vulkan_ctx->imgui_pool_)
    {
        vkDestroyDescriptorPool(vulkan_ctx->device_, vulkan_ctx->imgui_pool_, nullptr);
    }
#endif
    vulkan_ctx->transfer_pool_ = nullptr;
    for (auto img : vulkan_ctx->swapchain_images_)
    {
        Delete(img);
    }
    for (auto img_view : vulkan_ctx->swapchain_image_views_)
    {
        Delete(img_view);
    }
    vulkan_ctx->swapchain_images_.Clear();
    vulkan_ctx->swapchain_image_views_.Clear();
    vulkan_ctx->swapchain_image_desc_ = ImageDesc::Default();
}

void GfxContext_Vulkan::WaitForDeviceIdle()
{
    vkDeviceWaitIdle(device_);
}

void GfxContext_Vulkan::WaitForQueueExecution(QueueFamilyType type)
{
    vkQueueWaitIdle(GetQueue(type));
}

UniquePtr<Pipeline> GfxContext_Vulkan::CreateGraphicsPipeline(const GraphicsPipelineDesc& create_info, NRHI::RenderPass* render_pass)
{
    return MakeUnique<GraphicsPipeline_Vulkan>(create_info, render_pass);
}

Optional<int32_t> GfxContext_Vulkan::GetCurrentSwapChainImageIndexSync(Semaphore* signal_semaphore = nullptr)
{
    uint32_t image_idx;
    VkSemaphore semaphore = VK_NULL_HANDLE;
    if (signal_semaphore != nullptr)
    {
        semaphore = signal_semaphore->GetNativeHandleT<VkSemaphore>();
    }
    VkResult result = vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, semaphore, VK_NULL_HANDLE, &image_idx);
    if (result == VK_SUCCESS)
    {
        return MakeOptional<int32_t>(image_idx);
    }
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return {};
    }
    Log(Fatal) << "获取交换链图像索引失败: "_es + VulkanErrorToString(result);
    return {};
}

SharedPtr<DescriptorSetLayout> GfxContext_Vulkan::CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc)
{
    auto rtn = MakeShared<DescriptorSetLayout_Vulkan>(desc);
    // #ifdef ELBOW_DEBUG
    //     VkDebugUtilsObjectNameInfoEXT name_info{};
    //     name_info.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    //     name_info.objectType   = VK_OBJECT_TYPE_SHADER_MODULE;
    //     name_info.objectHandle = reinterpret_cast<uint64_t>(rtn->GetNativeHandleT<VkDescriptorSetLayout>());
    //     name_info.pObjectName  = *debug_name;
    //     SetDebugUtilsObjectNameEXT(device_, &name_info);
    // #endif
    return rtn;
}

UniquePtr<Semaphore> GfxContext_Vulkan::Create_Semaphore(uint64_t init_value, bool timeline)
{
    return MakeUnique<Semaphore_Vulkan>(init_value, timeline);
}

static bool InternalPresent(uint32_t image_index, const Semaphore* wait_semaphore)
{
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    VkSemaphore semaphore = VK_NULL_HANDLE;
    if (wait_semaphore != nullptr)
    {
        present_info.waitSemaphoreCount = 1;
        semaphore = wait_semaphore->GetNativeHandleT<VkSemaphore>();
        present_info.pWaitSemaphores = &semaphore;
    }
    present_info.swapchainCount = 1;
    VkSwapchainKHR swapchain = GetVulkanGfxContext()->GetSwapchain();
    present_info.pSwapchains = &swapchain;
    present_info.pImageIndices = &image_index;
    VkResult result = vkQueuePresentKHR(GetVulkanGfxContext()->GetQueue(QueueFamilyType::Present), &present_info);
    if (result == VK_SUCCESS)
    {
        return true;
    }
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return false;
    }
    Log(Fatal) << "交换链呈现操作提交失败: "_es + VulkanErrorToString(result);
    return false;
}

bool GfxContext_Vulkan::Present(uint32_t image_index, Semaphore* wait_semaphore)
{
    return InternalPresent(image_index, wait_semaphore);
}

ImageView* GfxContext_Vulkan::GetSwapChainView(UInt32 index)
{
    Assert(index < swapchain_image_views_.Count(), "交换链索引超出范围");
    return swapchain_image_views_[index];
}

Image* GfxContext_Vulkan::GetSwapChainImage(UInt32 index)
{
    Assert(index < swapchain_images_.Count(), "交换链索引超出范围");
    return swapchain_images_[index];
}

GfxContext_Vulkan* NRHI::GetVulkanGfxContext()
{
    return static_cast<GfxContext_Vulkan*>(GetGfxContext());
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
{
    const char* message_type = nullptr;
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
    {
        message_type = "General";
    }
    else if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
    {
        message_type = "Validation";
    }
    else if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
    {
        message_type = "Performance";
    }
    else if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT)
    {
        message_type = "Device Address Binding";
    }
    else
    {
        message_type = "Unknown";
    }
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        Log(Error) << String::Format("[{}] {}", message_type, callback_data->pMessage);
    }
    else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        Log(Warn) << String::Format("[{}] {}", message_type, callback_data->pMessage);
    }
    else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        Log(Info) << String::Format("[{}] {}", message_type, callback_data->pMessage);
    }
    else
    {
    }
    return VK_FALSE;
}

static void CreateInstance(VkInstance& instance)
{
    ProfileScope _(__func__);
    auto* rhi_cfg = GetConfig<PlatformConfig>();
    auto* core_cfg = GetConfig<CoreConfig>();
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = core_cfg->GetAppName().Data();
    app_info.applicationVersion = VK_MAKE_VERSION(core_cfg->GetAppVersion().Major, core_cfg->GetAppVersion().Minor, core_cfg->GetAppVersion().Patch);
    app_info.pEngineName = "Elbow Engine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

    if (rhi_cfg->GetValidEnableValidationLayer())
    {
        required_instance_extensions.Add("VK_EXT_debug_utils");
    }
    required_instance_extensions = Evt_PostProcessVulkanExtensions.InvokeOnce(required_instance_extensions);
    for (auto& extension : required_instance_extensions)
    {
        Log(Info) << "  Enabled instance extension: "_es + extension;
    }

    VkInstanceCreateInfo instance_info{};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    const auto required_extension_cstr =
        required_instance_extensions | NRange::NView::Select([](const String& str) { return str.Data(); }) | NRange::To<Array<const char*>>();
    instance_info.enabledExtensionCount = required_extension_cstr.Count();
    instance_info.ppEnabledExtensionNames = required_extension_cstr.Data();
    Log(Info) << "  Enabled validation layer: "_es + String::FromBool(rhi_cfg->GetValidEnableValidationLayer());
    VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
    if (rhi_cfg->GetValidEnableValidationLayer())
    {
        Assert(SupportValidationLayer("VK_LAYER_KHRONOS_validation"), "Validation layer is not supported!");
        const char* validation_layer_names = "VK_LAYER_KHRONOS_validation";
        debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_info.pfnUserCallback = DebugCallback;
        instance_info.enabledLayerCount = 1;
        instance_info.ppEnabledLayerNames = &validation_layer_names;
        instance_info.pNext = &debug_info;
    }
    else
    {
        instance_info.enabledLayerCount = 0;
        instance_info.pNext = nullptr;
    }
    VerifyVulkanResult(vkCreateInstance(&instance_info, nullptr, AddressOf(instance)));
}

static void DestroyInstance(VkInstance& instance)
{
    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;
}

static void CreateSurface(Surface*& surface, const VkInstance instance)
{
    const auto window = PlatformWindowManager::GetMainWindow();
    Assert(window != nullptr, "Window must be initialized before create surface!");
    surface = window->CreateSurface(instance, GraphicsAPI::Vulkan);
}

static void DestroySurface(Surface*& surface)
{
    const auto window = PlatformWindowManager::GetMainWindow();
    Assert(window != nullptr, "Window must be destroyed after surface destroyed!");
    window->DestroySurface(surface);
    surface = nullptr;
}

static bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t cnt;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &cnt, nullptr);
    Array<VkExtensionProperties> extensions;
    extensions.Resize(cnt);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &cnt, extensions.Data());
    Array my_required_extensions = {"VK_KHR_swapchain"};
    for (Int32 i = my_required_extensions.Count() - 1; i >= 0; --i)
    {
        if (NRange::AnyOf(extensions,
                         [ext = String(my_required_extensions[i])](const VkExtensionProperties& prop) { return ext == prop.extensionName; }))
        {
            my_required_extensions.FastRemoveAt(i);
        }
    }
    return my_required_extensions.Empty();
}

static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, const Surface* surface)
{
    QueueFamilyIndices indices;
    UInt32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    Array<VkQueueFamilyProperties> queue_families;
    queue_families.Resize(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.Data());
    int i = 0;
    for (const auto& queue_family : queue_families)
    {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = i;
        }
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, static_cast<VkSurfaceKHR>(surface->GetNativeHandle()), &present_support);
        if (present_support)
        {
            indices.present_family = i;
        }
        if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            indices.transfer_family = i;
        }
        if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            indices.compute_family = i;
        }
        if (indices.IsComplete())
        {
            break;
        }
        ++i;
    }
    return indices;
}

static bool IsDeviceSuitable(VkPhysicalDevice device, Surface* surface)
{
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);
    if (!indices.IsComplete())
        return false;
    if (!CheckDeviceExtensionSupport(device))
        return false;
    auto support = QuerySwapChainSupportInfo(device, surface);
    if (support.formats.Empty() || support.present_modes.Empty())
        return false;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);
    return features.samplerAnisotropy;
}

static void SelectPhysicalDevice(VkPhysicalDevice& physical_device, VkInstance instance, Surface* surface)
{
    ProfileScope _(__func__);
    if (physical_devices.Empty())
    {
        physical_devices = GetAvailablePhysicalDevices(instance);
    }
    for (auto candidate : physical_devices)
    {
        if (IsDeviceSuitable(candidate, surface))
        {
            physical_device = candidate;
            break;
        }
    }
    Assert(physical_device != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
}

static void CreateLogicalDevice(VkPhysicalDevice physical_device, Surface* surface, OUT VkDevice& device, OUT VkQueue& graphics_queue,
                                OUT VkQueue& present_queue, OUT VkQueue& transfer_queue, OUT VkQueue& compute_queue)
{
    auto indices = FindQueueFamilies(physical_device, surface);
    Assert(indices.IsComplete(), "Find uncompleted queue families.");
    Array<VkDeviceQueueCreateInfo> queue_create_infos;
    queue_create_infos.Reserve(2);
    Array unique_queue_families =
        Array{*indices.graphics_family, *indices.present_family, *indices.transfer_family, *indices.compute_family}.Unique();
    float queue_priority = 1.0f;
    for (UInt32 queue_family : unique_queue_families)
    {
        VkDeviceQueueCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        create_info.queueFamilyIndex = queue_family;
        create_info.queueCount = 1;
        create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.Add(create_info);
    }

    // 开启特性Dynamic Rendering
    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features = {};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.Count());
    create_info.pQueueCreateInfos = queue_create_infos.Data();
    create_info.pNext = &dynamic_rendering_features;
    // TODO: 这里改成由配置文件配置设备特性
    auto cfg = GetConfig<PlatformConfig>();
    VkPhysicalDeviceFeatures features = {};
    features.samplerAnisotropy = VK_TRUE;
    create_info.pEnabledFeatures = &features;
    Array required_extension_cstrs = {"VK_KHR_swapchain"};
    create_info.enabledExtensionCount = required_extension_cstrs.Count();
    create_info.ppEnabledExtensionNames = required_extension_cstrs.Data();
    Array layers = {"VK_LAYER_KHRONOS_validation"};
    if (cfg->GetValidEnableValidationLayer())
    {
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = layers.Data();
    }
    else
    {
        create_info.enabledLayerCount = 0;
    }
    VerifyVulkanResult(vkCreateDevice(physical_device, &create_info, nullptr, &device));
    vkGetDeviceQueue(device, *indices.graphics_family, 0, &graphics_queue);
    vkGetDeviceQueue(device, *indices.present_family, 0, &present_queue);
    vkGetDeviceQueue(device, *indices.transfer_family, 0, &transfer_queue);
    vkGetDeviceQueue(device, *indices.compute_family, 0, &compute_queue);
}

static void DestroyLogicalDevice(VkDevice& device)
{
    vkDestroyDevice(device, nullptr);
    device = nullptr;
}

static Format CreateSwapChain(const SwapChainSupportInfo& swapchain_support, const Surface* surface, VkPhysicalDevice physical_device,
                              VkDevice device, ImageDesc& desc, Vector2f size, OUT VkSwapchainKHR& swapchain)
{
    const auto cfg = GetConfig<PlatformConfig>();
    auto surface_handle = static_cast<VkSurfaceKHR>(surface->GetNativeHandle());

    SurfaceFormat available_format{};
    for (auto& surface_format : swapchain_support.formats)
    {
        if (surface_format.format == Format::B8G8R8A8_UNorm && surface_format.color_space == ColorSpace::sRGB)
        {
            available_format = surface_format;
        }
    }
    const PresentMode present_mode = cfg->GetPresentMode();
    const uint32_t image_count = cfg->GetSwapchainImageCount();
    Assert(image_count <= swapchain_support.max_image_count && image_count >= swapchain_support.min_image_count,
           "Unsupported swap chain image count");
    VkSwapchainCreateInfoKHR swapchain_create_info{};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = surface_handle;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapchain_create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchain_create_info.imageExtent = VkExtent2D{.width = static_cast<UInt32>(size.X), .height = static_cast<UInt32>(size.Y)};
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    desc.DepthOrLayers = 1;
    desc.Format = Format::B8G8R8A8_UNorm;
    desc.Height = size.X;
    desc.Width = size.Y;
    desc.MipLevels = 1;
    desc.InitialState = ImageLayout::Undefined;

    QueueFamilyIndices indices = FindQueueFamilies(physical_device, surface);
    uint32_t queue_family_indices[] = {*indices.graphics_family, *indices.present_family};
    if (indices.graphics_family != indices.present_family)
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = RHIPresentModeToVkPresentMode(present_mode);
    swapchain_create_info.clipped = VK_TRUE;
    VerifyVulkanResult(vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, AddressOf(swapchain)));

    return VkFormatToRHIFormat(swapchain_create_info.imageFormat);
}

static void DestroySwapChain(VkDevice device, VkSwapchainKHR& swapchain)
{
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    swapchain = nullptr;
}

GfxContext_Vulkan::GfxContext_Vulkan()
{
    post_vulkan_gfx_context_init_ = Evt_OnGfxContextPostInitialized.AddBind(&ThisType::PostVulkanGfxContextInit);
    pre_vulkan_gfx_context_destroyed_ = Evt_OnGfxContextPreDestroyed.AddBind(&ThisType::PreVulkanGfxContextDestroyed);
    available_layers = GetAvailableLayers();
    Log(Info) << "Initializing Vulkan Graphics API...";
    CreateInstance(instance_);
    CreateSurface(surface_, instance_);
    SelectPhysicalDevice(physical_device_, instance_, surface_);
    queue_family_indices_ = FindQueueFamilies(physical_device_, surface_);
    CreateLogicalDevice(physical_device_, surface_, device_, graphics_queue_, present_queue_, transfer_queue_, compute_queue_);
    FindVulkanExtensionSymbols();
    auto cfg = GetConfig<PlatformConfig>();
    default_color_format_ = CreateSwapChain(QuerySwapChainSupportInfo(), surface_, physical_device_, device_, swapchain_image_desc_,
                                            cfg->GetDefaultWindowSize(), swapchain_);
    Format depth_format = FindSupportedFormat({Format::D32_Float, Format::D32_Float_S8X24_UInt, Format::D24_UNorm_S8_UInt}, VK_IMAGE_TILING_OPTIMAL,
                                              VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    Assert(depth_format != Format::Count, "Failed to find supported depth format!");
    default_depth_stencil_format_ = depth_format;

    auto [name, limits] = QueryDeviceInfo();
    Log(Info) << "Using device: "_es + name;
}

void GfxContext_Vulkan::ResizeSwapChain(Int32 width, Int32 height)
{
    for (auto& image_view : swapchain_image_views_)
    {
        Delete(image_view);
    }
    swapchain_image_views_.Clear();
    swapchain_images_.Clear();
    DestroySwapChain(device_, swapchain_);
    default_color_format_ = CreateSwapChain(QuerySwapChainSupportInfo(), surface_, physical_device_, device_, swapchain_image_desc_,
                                            Vector2f{static_cast<Float>(width), static_cast<Float>(height)}, swapchain_);
    uint32_t img_cnt = 0;
    VerifyVulkanResult(vkGetSwapchainImagesKHR(device_, swapchain_, &img_cnt, nullptr));
    auto vk_imgs = Array<VkImage>{};
    vk_imgs.Resize(img_cnt);
    VerifyVulkanResult(vkGetSwapchainImagesKHR(device_, swapchain_, &img_cnt, vk_imgs.Data()));
    if (img_cnt <= 0)
    {
        Log(Fatal) << "创建交换链失败";
    }
    swapchain_images_ = //
        vk_imgs | NRange::NView::Enumerate | NRange::NView::Select([this](const auto& pair) {
            auto& desc = swapchain_image_desc_;
            const auto& [idx, img] = pair;
            return New<Image_Vulkan>(img, idx, desc.Width, desc.Height, desc.Format);
        }) |
        NRange::To<Array<Image_Vulkan*>>();
    swapchain_image_views_ = //
        swapchain_images_ | NRange::NView::Enumerate | NRange::NView::Select([](const auto& pair) {
            const auto& [idx, img] = pair;
            ImageViewDesc desc{img};
            return New<ImageView_Vulkan>(desc);
        }) |
        NRange::To<Array<ImageView_Vulkan*>>();
    for (const auto framebuffer : imgui_framebuffers_)
    {
        vkDestroyFramebuffer(device_, framebuffer, nullptr);
    }
    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = imgui_render_pass_;
    framebuffer_create_info.width = width;
    framebuffer_create_info.height = height;
    framebuffer_create_info.layers = 1;
    framebuffer_create_info.attachmentCount = 1;
    imgui_framebuffers_.Resize(swapchain_image_views_.Count());
    for (UInt32 i = 0; i < imgui_framebuffers_.Count(); i++)
    {
        VkImageView view = swapchain_image_views_[i]->GetNativeHandleT<VkImageView>();
        framebuffer_create_info.pAttachments = &view;
        VerifyVulkanResult(vkCreateFramebuffer(device_, &framebuffer_create_info, nullptr, &imgui_framebuffers_[i]));
    }
}

SharedPtr<DescriptorSetPool> GfxContext_Vulkan::CreateDescriptorSetPool(const DescriptorSetPoolDesc& desc)
{
    return MakeShared<DescriptorSetPool_Vulkan>(desc);
}

void GfxContext_Vulkan::BeginImGuiFrame(NRHI::CommandBuffer& cmd, Int32 img_index, Int32 w, Int32 h)
{
    auto Buffer = cmd.GetNativeHandleT<VkCommandBuffer>();
    const auto Cfg = GetConfig<PlatformConfig>();
    const Color ClearColor = Cfg->GetDefaultClearColor();
    auto Task = Just() | Then([Buffer, this, img_index, w, h, ClearColor]() {
                    ImGui_ImplVulkan_NewFrame();
                    VkDebugUtilsLabelEXT DebugLabelInfo = {};
                    DebugLabelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
                    DebugLabelInfo.pLabelName = "ImGui Pass";
                    BeginDebugLabel(Buffer, DebugLabelInfo);
                    VkRenderPassBeginInfo Info = {};
                    VkClearValue ClearValue;
                    ClearValue.color = VkClearColorValue{ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A};
                    Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                    Info.renderPass = imgui_render_pass_;
                    Info.framebuffer = imgui_framebuffers_[img_index];
                    Info.renderArea.extent.width = w;
                    Info.renderArea.extent.height = h;
                    Info.clearValueCount = 1;
                    Info.pClearValues = &ClearValue;
                    vkCmdBeginRenderPass(Buffer, &Info, VK_SUBPASS_CONTENTS_INLINE);
                });
    if (Cfg->GetEnableMultithreadRender())
    {
        ThreadManager::ScheduleFutureAsync(Task, NamedThread::Render);
    }
    else
    {
        ThreadManager::ScheduleFutureAsync(Task, NamedThread::Game, true);
    }
}

void GfxContext_Vulkan::EndImGuiFrame(NRHI::CommandBuffer& buffer)
{
    auto cfg = GetConfig<PlatformConfig>();
    auto task = Just() | Then([&buffer]() {
                    ImGui::Render();
                    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer.GetNativeHandleT<VkCommandBuffer>());
                    vkCmdEndRenderPass(buffer.GetNativeHandleT<VkCommandBuffer>());
                });
    if (cfg->GetEnableMultithreadRender())
    {
        ThreadManager::ScheduleFutureAsync(task, NamedThread::Render);
    }
    else
    {
        ThreadManager::ScheduleFutureAsync(task, NamedThread::Game, true);
    }
}

SharedPtr<Sampler> GfxContext_Vulkan::CreateSampler(const SamplerDesc& desc, StringView debug_name)
{
    auto rtn = MakeShared<Sampler_Vulkan>(desc);
#ifdef ELBOW_DEBUG
    if (!debug_name.IsEmpty())
    {
        SetObjectDebugName(VK_OBJECT_TYPE_SAMPLER, rtn->GetNativeHandle(), debug_name);
    }
#endif
    return rtn;
}

GfxContext_Vulkan::~GfxContext_Vulkan()
{
    DestroySwapChain(device_, swapchain_);
    DestroySurface(surface_);
    DestroyLogicalDevice(device_);
    DestroyInstance(instance_);
}
