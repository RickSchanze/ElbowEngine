/**
 * @file GfxContext_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#include "GfxContext_Vulkan.h"

#include "Buffer_Vulkan.h"
#include "CommandBuffer_Vulkan.h"
#include "Core/Async/Execution/StartAsync.h"
#include "Core/Async/Execution/Then.h"
#include "Core/Base/EString.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Core/Profiler/ProfileMacro.h"
#include "DescriptorSet_Vulkan.h"
#include "Enums_Vulkan.h"
#include "Image_Vulkan.h"
#include "ImageView_Vulkan.h"
#include "LowShader_Vulkan.h"
#include "Pipeline_Vulkan.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/PlatformLogcat.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Surface.h"
#include "Platform/Window/WindowManager.h"
#include "range/v3/all.hpp"
#include "SyncPrimitives_Vulkan.h"

static core::Array<VkLayerProperties> available_layers;
static core::Array<VkPhysicalDevice>  physical_devices;
static core::Array<core::String>      required_instance_extensions;

using namespace ranges::views;
using namespace core::exec;
using namespace core;

static core::Array<VkPhysicalDevice> GetAvailablePhysicalDevices(VkInstance instance)
{
    uint32_t device_cnt = 0;
    vkEnumeratePhysicalDevices(instance, &device_cnt, nullptr);
    core::Assert::Require(logcat::Platform_RHI_Vulkan, device_cnt > 0, "No physical device found");
    core::Array<VkPhysicalDevice> physical_devices(device_cnt);
    vkEnumeratePhysicalDevices(instance, &device_cnt, physical_devices.data());
    return physical_devices;
}

static core::Array<VkLayerProperties> GetAvailableLayers()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    core::Array<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
    return available_layers;
}

static bool SupportValidationLayer(core::StringView layer_name)
{
    for (auto& layer: available_layers)
    {
        if (layer_name == layer.layerName)
        {
            return true;
        }
    }
    return false;
}

namespace platform::rhi::vulkan
{
GraphicsAPI GfxContext_Vulkan::GetAPI() const
{
    return GraphicsAPI::Vulkan;
}

core::Array<VkExtensionProperties> GfxContext_Vulkan::GetAvailableExtensions(VkPhysicalDevice)
{
    uint32_t extension_count;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    core::Array<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data());
    return available_extensions;
}

static core::Size2D FromVkExtent2D(const VkExtent2D& extent)
{
    core::Size2D size{};
    size.width  = extent.width;
    size.height = extent.height;
    return size;
}

static SwapChainSupportInfo QuerySwapChainSupportInfo(const VkPhysicalDevice device, const Surface* surface)
{
    SwapChainSupportInfo     info;
    const auto               surfaceVk = static_cast<VkSurfaceKHR>(surface->GetNativeHandle());
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surfaceVk, &capabilities);
    info.current_extent         = FromVkExtent2D(capabilities.currentExtent);
    info.min_image_extent       = FromVkExtent2D(capabilities.minImageExtent);
    info.max_image_extent       = FromVkExtent2D(capabilities.maxImageExtent);
    info.min_image_count        = capabilities.minImageCount;
    info.max_image_count        = capabilities.maxImageCount;
    info.max_image_array_layers = capabilities.maxImageArrayLayers;

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceVk, &format_count, nullptr);
    if (format_count != 0)
    {
        core::Array<VkSurfaceFormatKHR> formats(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceVk, &format_count, formats.data());
        info.formats =
            formats | transform([](const VkSurfaceFormatKHR& format) {
                return SurfaceFormat{.format = VkFormatToRHIFormat(format.format), .color_space = VkColorSpaceToRHIColorSpace(format.colorSpace)};
            }) |
            ranges::to_vector;
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceVk, &present_mode_count, nullptr);
    if (present_mode_count != 0)
    {
        core::Array<VkPresentModeKHR> present_modes(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceVk, &present_mode_count, present_modes.data());
        info.present_modes = present_modes | transform([](VkPresentModeKHR mode) { return VkPresentModeToRHIPresentMode(mode); }) | ranges::to_vector;
    }
    return info;
}

SwapChainSupportInfo GfxContext_Vulkan::QuerySwapChainSupportInfo()
{
    return vulkan::QuerySwapChainSupportInfo(physical_device_, surface_);
}

const PhysicalDeviceFeature& GfxContext_Vulkan::QueryDeviceFeature()
{
    if (device_feature_) return *device_feature_;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physical_device_, &features);
    PhysicalDeviceFeature feature;
    feature.sampler_anisotropy = features.samplerAnisotropy;
    device_feature_            = feature;
    return *device_feature_;
}

const PhysicalDeviceInfo& GfxContext_Vulkan::QueryDeviceInfo()
{
    if (device_info_) return *device_info_;
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device_, &properties);
    PhysicalDeviceInfo info;
    info.name                                  = properties.deviceName;
    info.limits.framebuffer_color_sample_count = static_cast<int32_t>(properties.limits.framebufferColorSampleCounts);
    info.limits.framebuffer_depth_sample_count = static_cast<int32_t>(properties.limits.framebufferDepthSampleCounts);
    device_info_                               = info;
    LOGGER.Info(logcat::Test, "Uniform alignment: {}", properties.limits.minUniformBufferOffsetAlignment);
    return *device_info_;
}

Format GfxContext_Vulkan::GetDefaultDepthStencilFormat() const
{
    return default_depth_stencil_format_;
}

Format GfxContext_Vulkan::GetDefaultColorFormat() const
{
    return default_color_format_;
}

const QueueFamilyIndices& GfxContext_Vulkan::GetCurrentQueueFamilyIndices() const
{
    return queue_family_indices_;
}

VkImageView GfxContext_Vulkan::CreateImageView_VK(const ImageViewDesc& desc) const
{
    const auto img        = desc.image;
    const auto img_handle = static_cast<VkImage>(img->GetNativeHandle());
    core::Assert::Require(logcat::Platform_RHI_Vulkan, img, "Image cannot be nullptr when creating a ImageView.");
    VkImageViewCreateInfo view_info{};
    view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image                           = img_handle;
    view_info.components                      = FromComponentMapping(desc.component_mapping);
    view_info.format                          = RHIFormatToVkFormat(desc.format);
    view_info.subresourceRange.aspectMask     = RHIImageAspectToVkImageAspect(desc.subresource_range.aspect_mask);
    view_info.subresourceRange.baseMipLevel   = desc.subresource_range.base_mip_level;
    view_info.subresourceRange.levelCount     = desc.subresource_range.level_count;
    view_info.subresourceRange.baseArrayLayer = desc.subresource_range.base_array_layer;
    view_info.subresourceRange.layerCount     = desc.subresource_range.layer_count;
    view_info.viewType                        = RHIImageDimensionToVkImageViewType(desc.type);
    view_info.pNext                           = nullptr;
    VkImageView view_handle                   = VK_NULL_HANDLE;
    if (const VkResult result = vkCreateImageView(device_, &view_info, nullptr, &view_handle); result != VK_SUCCESS)
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "Failed to create ImageView: {}", VulkanErrorToString(result));
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
    buffer_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size        = size;
    buffer_info.usage       = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkBuffer       buffer;
    const VkResult result = vkCreateBuffer(device_, &buffer_info, nullptr, &buffer);
    VERIFY_VULKAN_RESULT(result);
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
    alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize  = mem_requirements.size;
    alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);
    VkDeviceMemory memory;
    const VkResult result = vkAllocateMemory(device_, &alloc_info, nullptr, &memory);
    VERIFY_VULKAN_RESULT(result);
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
    const auto    result = vkCreateCommandPool(device_, &info, nullptr, &command_pool_);
    VERIFY_VULKAN_RESULT(result);
    return command_pool_;
}

void GfxContext_Vulkan::DestroyCommandPool_VK(VkCommandPool pool) const
{
    vkDestroyCommandPool(device_, pool, nullptr);
}

void GfxContext_Vulkan::CreateCommandBuffers_VK(const VkCommandBufferAllocateInfo& alloc_info, VkCommandBuffer* command_buffers) const
{
    const VkResult result = vkAllocateCommandBuffers(device_, &alloc_info, command_buffers);
    VERIFY_VULKAN_RESULT(result);
}

core::UniquePtr<Fence> GfxContext_Vulkan::CreateFence(bool signaled)
{
    return core::MakeUnique<Fence_Vulkan>(signaled);
}

core::SharedPtr<LowShader> GfxContext_Vulkan::CreateShader(const char* code, size_t size, core::StringView debug_name)
{
    auto rtn = core::MakeShared<LowShader_Vulkan>(code, size);
#if ELBOW_DEBUG
    SetObjectDebugName(VK_OBJECT_TYPE_SHADER_MODULE, rtn->GetNativeHandleT<VkShaderModule>(), debug_name);
#endif
    return rtn;
}

static void InternalSubmit(const SharedPtr<CommandBuffer>& buffer, const SubmitParameter& parameter)
{
    auto*        ctx = GetVulkanGfxContext();
    VkSubmitInfo submit_info{};
    submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    VkCommandBuffer command_buffer = buffer->GetNativeHandleT<VkCommandBuffer>();
    submit_info.pCommandBuffers    = &command_buffer;
    VkTimelineSemaphoreSubmitInfo timeline_submit_info{};
    timeline_submit_info.sType   = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    VkSemaphore signal_semaphore = VK_NULL_HANDLE;
    VkSemaphore wait_semaphore   = VK_NULL_HANDLE;
    if (parameter.signal_semaphore != nullptr)
    {
        if (parameter.signal_semaphore->Vulkan_IsTimelineSemaphore())
        {
            timeline_submit_info.signalSemaphoreValueCount = 1;
            timeline_submit_info.pSignalSemaphoreValues    = &parameter.signal_value;
        }
        submit_info.signalSemaphoreCount = 1;
        signal_semaphore                 = parameter.signal_semaphore->GetNativeHandleT<VkSemaphore>();
        submit_info.pSignalSemaphores    = &signal_semaphore;
    }
    if (parameter.wait_semaphore != nullptr)
    {
        if (parameter.wait_semaphore->Vulkan_IsTimelineSemaphore())
        {
            timeline_submit_info.waitSemaphoreValueCount = 1;
            timeline_submit_info.pWaitSemaphoreValues    = &parameter.wait_value;
        }
        submit_info.waitSemaphoreCount = 1;
        wait_semaphore                 = parameter.wait_semaphore->GetNativeHandleT<VkSemaphore>();
        submit_info.pWaitSemaphores    = &wait_semaphore;
    }
    submit_info.pNext                = &timeline_submit_info;
    // TODO: wait_stages参数化
    VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit_info.pWaitDstStageMask    = &wait_stages;
    VkFence fence                    = VK_NULL_HANDLE;
    if (parameter.fence != nullptr)
    {
        fence = parameter.fence->GetNativeHandleT<VkFence>();
    }
    vkQueueSubmit(ctx->GetQueue(parameter.submit_queue_type), 1, &submit_info, fence);
}

AsyncResultHandle<> GfxContext_Vulkan::Submit(core::SharedPtr<CommandBuffer> buffer, const SubmitParameter& parameter)
{
    auto* cfg = core::GetConfig<PlatformConfig>();
    if (cfg->GetEnableMultithreadRender())
    {
        auto& scheduler = core::ThreadManager::GetScheduler();
        auto  task      = Schedule(scheduler, core::ThreadSlot::Render) | Then([buffer, &parameter] { InternalSubmit(buffer, parameter); });
        return StartAsync(task);
    }
    else
    {
        InternalSubmit(buffer, parameter);
        return MakeAsyncResult();
    }
}

core::SharedPtr<Buffer> GfxContext_Vulkan::CreateBuffer(const BufferDesc& create_info, StringView name)
{
    auto rtn = core::MakeShared<Buffer_Vulkan>(create_info);
#if ELBOW_DEBUG
    if (!name.IsEmpty())
    {
        SetObjectDebugName(VK_OBJECT_TYPE_BUFFER, rtn->GetNativeHandle(), *name);
    }
#endif
    return rtn;
}

core::SharedPtr<CommandPool> GfxContext_Vulkan::CreateCommandPool(const CommandPoolCreateInfo& create_info)
{
    return core::MakeShared<CommandPool_Vulkan>(create_info);
}

void GfxContext_Vulkan::SetObjectDebugName(const VkObjectType type, void* handle, const core::StringView name) const
{
#if ELBOW_DEBUG
    if (core::GetConfig<PlatformConfig>()->GetValidEnableValidationLayer() == false) return;
    VkDebugUtilsObjectNameInfoEXT name_info = {};
    name_info.sType                         = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    name_info.objectType                    = type;
    name_info.objectHandle                  = reinterpret_cast<uint64_t>(handle);
    name_info.pObjectName                   = name.Data();
    if (const VkResult result = SetDebugUtilsObjectNameEXT(device_, &name_info); result != VK_SUCCESS)
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "Failed to set debug name for object: {}", VulkanErrorToString(result));
    }
#endif
}

uint32_t GfxContext_Vulkan::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) const
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
    throw RHIException("未找到合适的内存类型");
}

void GfxContext_Vulkan::BeginDebugLabel(VkCommandBuffer cmd, const VkDebugUtilsLabelEXT& info) const
{
    if (core::GetConfig<PlatformConfig>()->GetValidEnableValidationLayer() == false) return;
    CmdBeginDebugUtilsLabelEXT(cmd, &info);
}

void GfxContext_Vulkan::EndDebugLabel(VkCommandBuffer cmd) const
{
    if (core::GetConfig<PlatformConfig>()->GetValidEnableValidationLayer() == false) return;
    CmdEndDebugUtilsLabelEXT(cmd);
}

VkQueue GfxContext_Vulkan::GetQueue(QueueFamilyType type) const
{
    switch (type)
    {
    case QueueFamilyType::Graphics: return graphics_queue_; break;
    case QueueFamilyType::Compute: throw core::NotSupportException("Compute queue尚不支持"); break;
    case QueueFamilyType::Transfer: return transfer_queue_; break;
    case QueueFamilyType::Present: return present_queue_;
    }
    return VK_NULL_HANDLE;
}

core::SharedPtr<Image> GfxContext_Vulkan::CreateImage(const ImageDesc& desc, core::StringView debug_name)
{
    auto rtn = core::MakeShared<Image_Vulkan>(desc);
#if ELBOW_DEBUG
    if (!debug_name.IsEmpty())
    {
        SetObjectDebugName(VK_OBJECT_TYPE_IMAGE, rtn->GetNativeHandle(), debug_name);
    }
#endif
    return rtn;
}

SharedPtr<ImageView> GfxContext_Vulkan::CreateImageView(const ImageViewDesc& desc, StringView debug_name)
{
    auto rtn = core::MakeShared<ImageView_Vulkan>(desc);
#if ELBOW_DEBUG
    if (!debug_name.IsEmpty())
    {
        SetObjectDebugName(VK_OBJECT_TYPE_IMAGE_VIEW, rtn->GetNativeHandle(), debug_name);
    }
#endif
    return rtn;
}

Format GfxContext_Vulkan::FindSupportedFormat(const core::Array<Format>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
    for (const core::Array<VkFormat> fmts = candidates | transform([](Format fmt) { return RHIFormatToVkFormat(fmt); }) | ranges::to_vector;
         const auto                  fmt: fmts)
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
    CmdEndDebugUtilsLabelEXT   = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetDeviceProcAddr(device_, "vkCmdEndDebugUtilsLabelEXT"));
}

constexpr const char* swapchain_image_view_names_[] = {
    "SwapChainImageView0",
    "SwapChainImageView1",
    "SwapChainImageView2",
    "SwapChainImageView3",
    "SwapChainImageView4",
    "SwapChainImageView5",
    "SwapChainImageView6",
    "SwapChainImageView7",
    "SwapChainImageView8",   // 开发设备最多支持八个
};

void GfxContext_Vulkan::PostVulkanGfxContextInit(GfxContext* ctx)
{

    auto vulkan_ctx = static_cast<GfxContext_Vulkan*>(ctx);

    uint32_t img_cnt = 0;
    VERIFY_VULKAN_RESULT(vkGetSwapchainImagesKHR(vulkan_ctx->device_, vulkan_ctx->swapchain_, &img_cnt, nullptr));
    auto vk_imgs = core::Array<VkImage>(img_cnt);
    VERIFY_VULKAN_RESULT(vkGetSwapchainImagesKHR(vulkan_ctx->device_, vulkan_ctx->swapchain_, &img_cnt, vk_imgs.data()));
    if (img_cnt <= 0)
    {
        LOGGER.Critical(logcat::Platform_RHI_Vulkan, "创建交换链获取失败");
    }
    vulkan_ctx->swapchain_images_ =   //
        vk_imgs | enumerate | transform([vulkan_ctx](const auto& pair) {
            auto& desc             = vulkan_ctx->swapchain_image_desc_;
            const auto& [idx, img] = pair;
            return New<Image_Vulkan>(img, idx, desc.width, desc.height, desc.format);
        }) |
        ranges::to_vector;
    vulkan_ctx->swapchain_image_views_ =   //
        vulkan_ctx->swapchain_images_ | enumerate | transform([](const auto& pair) {
            const auto& [idx, img] = pair;
            ImageViewDesc desc(img);
            return New<ImageView_Vulkan>(desc);
        }) |
        ranges::to_vector;

    Event_GfxContextPostInitialized.RemoveBind(vulkan_ctx->post_vulkan_gfx_context_init_);
    CommandPoolCreateInfo transfer_pool_info{QueueFamilyType::Transfer, true};
    vulkan_ctx->transfer_pool_ = vulkan_ctx->CreateCommandPool(transfer_pool_info);
}

void GfxContext_Vulkan::PreVulkanGfxContextDestroyed(GfxContext* ctx)
{
    auto vulkan_ctx = static_cast<GfxContext_Vulkan*>(ctx);
    vkDeviceWaitIdle(vulkan_ctx->device_);
    vulkan_ctx->transfer_pool_ = nullptr;
    for (auto img: vulkan_ctx->swapchain_images_)
    {
        Delete(img);
    }
    for (auto img_view: vulkan_ctx->swapchain_image_views_)
    {
        Delete(img_view);
    }
    vulkan_ctx->swapchain_images_.clear();
    vulkan_ctx->swapchain_image_views_.clear();
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

core::UniquePtr<GraphicsPipeline> GfxContext_Vulkan::CreateGraphicsPipeline(const GraphicsPipelineDesc& create_info, rhi::RenderPass* render_pass)
{
    return core::MakeUnique<GraphicsPipeline_Vulkan>(create_info, render_pass);
}

core::Optional<int32_t> GfxContext_Vulkan::GetCurrentSwapChainImageIndexSync(Semaphore* signal_semaphore = nullptr)
{
    uint32_t    image_idx;
    VkSemaphore semaphore = VK_NULL_HANDLE;
    if (signal_semaphore != nullptr)
    {
        semaphore = signal_semaphore->GetNativeHandleT<VkSemaphore>();
    }
    VkResult result = vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, semaphore, VK_NULL_HANDLE, &image_idx);
    if (result == VK_SUCCESS)
    {
        return core::MakeOptional<int32_t>(image_idx);
    }
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return {};
    }
    throw RHIException(core::String::Format("获取交换链图像索引失败: {}", VulkanErrorToString(result)));
}

core::SharedPtr<DescriptorSetLayout> GfxContext_Vulkan::CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc)
{
    auto rtn = core::MakeShared<DescriptorSetLayout_Vulkan>(desc);
    // #if ELBOW_DEBUG
    //     VkDebugUtilsObjectNameInfoEXT name_info{};
    //     name_info.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    //     name_info.objectType   = VK_OBJECT_TYPE_SHADER_MODULE;
    //     name_info.objectHandle = reinterpret_cast<uint64_t>(rtn->GetNativeHandleT<VkDescriptorSetLayout>());
    //     name_info.pObjectName  = *debug_name;
    //     SetDebugUtilsObjectNameEXT(device_, &name_info);
    // #endif
    return rtn;
}

core::UniquePtr<Semaphore> GfxContext_Vulkan::CreateASemaphore(uint64_t init_value, bool timeline)
{
    return core::MakeUnique<Semaphore_Vulkan>(init_value, timeline);
}

static bool InternalPresent(uint32_t image_index, Semaphore* wait_semaphore)
{
    VkPresentInfoKHR present_info{};
    present_info.sType    = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    VkSemaphore semaphore = VK_NULL_HANDLE;
    if (wait_semaphore != nullptr)
    {
        present_info.waitSemaphoreCount = 1;
        semaphore                       = wait_semaphore->GetNativeHandleT<VkSemaphore>();
        present_info.pWaitSemaphores    = &semaphore;
    }
    present_info.swapchainCount = 1;
    VkSwapchainKHR swapchain    = GetVulkanGfxContext()->GetSwapchain();
    present_info.pSwapchains    = &swapchain;
    present_info.pImageIndices  = &image_index;
    VkResult result             = vkQueuePresentKHR(GetVulkanGfxContext()->GetQueue(QueueFamilyType::Present), &present_info);
    if (result == VK_SUCCESS)
    {
        return true;
    }
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return false;
    }
    throw RHIException(core::String::Format("交换链呈现操作提交失败: {}", VulkanErrorToString(result)));
}

bool GfxContext_Vulkan::Present(uint32_t image_index, Semaphore* wait_semaphore)
{
    return InternalPresent(image_index, wait_semaphore);
}

ImageView* GfxContext_Vulkan::GetSwapChainView(UInt32 index)
{
    core::Assert::Require(logcat::Platform_RHI_Vulkan, index < swapchain_image_views_.size(), "交换链索引超出范围");
    return swapchain_image_views_[index];
}

Image* GfxContext_Vulkan::GetSwapChainImage(UInt32 index)
{
    core::Assert::Require(logcat::Platform_RHI_Vulkan, index < swapchain_images_.size(), "交换链索引超出范围");
    return swapchain_images_[index];
}

GfxContext_Vulkan* GetVulkanGfxContext()
{
    return static_cast<GfxContext_Vulkan*>(GetGfxContext());
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data
)
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
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "[{}] {}", message_type, callback_data->pMessage);
    }
    else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        LOGGER.Warn(logcat::Platform_RHI_Vulkan, "[{}] {}", message_type, callback_data->pMessage);
    }
    else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "[{}] {}", message_type, callback_data->pMessage);
    }
    else
    {
        LOGGER.Debug(logcat::Platform_RHI_Vulkan, "[{}] {}", message_type, callback_data->pMessage);
    }
    return VK_FALSE;
}

static void CreateInstance(core::Ref<VkInstance> instance)
{
    PROFILE_SCOPE_AUTO;
    auto*             rhi_cfg  = core::GetConfig<PlatformConfig>();
    auto*             core_cfg = core::GetConfig<core::CoreConfig>();
    VkApplicationInfo app_info{};
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName   = core_cfg->GetAppName().Data();
    app_info.applicationVersion = VK_MAKE_VERSION(core_cfg->GetAppVersion().major, core_cfg->GetAppVersion().minor, core_cfg->GetAppVersion().patch);
    app_info.pEngineName        = "Elbow Engine";
    app_info.engineVersion      = VK_MAKE_VERSION(0, 1, 0);
    app_info.apiVersion         = VK_API_VERSION_1_3;

    if (rhi_cfg->GetValidEnableValidationLayer())
    {
        required_instance_extensions.emplace_back("VK_EXT_debug_utils");
    }
    required_instance_extensions = Event_PostProcessVulkanExtensions.InvokeOnce(required_instance_extensions);
    for (auto& extension: required_instance_extensions)
    {
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "  Enabled instance extension: {}", extension);
    }

    VkInstanceCreateInfo instance_info{};
    instance_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    const auto required_extension_cstr =
        required_instance_extensions | transform([](const core::String& str) { return str.Data(); }) | ranges::to_vector;
    instance_info.enabledExtensionCount   = required_extension_cstr.size();
    instance_info.ppEnabledExtensionNames = required_extension_cstr.data();

    LOGGER.Info(logcat::Platform_RHI_Vulkan, "  Enable validation layer: {}", rhi_cfg->GetValidEnableValidationLayer());
    VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
    if (rhi_cfg->GetValidEnableValidationLayer())
    {
        core::Assert::Require(
            logcat::Platform_RHI_Vulkan, SupportValidationLayer("VK_LAYER_KHRONOS_validation"), "Validation layer is not supported!"
        );
        const char* validation_layer_names = "VK_LAYER_KHRONOS_validation";
        LOGGER.Info(logcat::Test, "Validation layer: {}", validation_layer_names);
        debug_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_info.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_info.pfnUserCallback        = DebugCallback;
        instance_info.enabledLayerCount   = 1;
        instance_info.ppEnabledLayerNames = &validation_layer_names;
        instance_info.pNext               = &debug_info;
    }
    else
    {
        instance_info.enabledLayerCount = 0;
        instance_info.pNext             = nullptr;
    }
    VERIFY_VULKAN_RESULT(vkCreateInstance(&instance_info, nullptr, instance.GetPtr()));
}

static void DestroyInstance(core::Ref<VkInstance> instance)
{
    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;
}

static void CreateSurface(Ref<Surface*> surface, const VkInstance instance)
{
    const auto window = WindowManager::Get()->GetMainWindow();
    core::Assert::Require(logcat::Platform_RHI_Vulkan, window != nullptr, "Window must be initialized before create surface!");
    surface = window->CreateSurface(instance, GraphicsAPI::Vulkan);
}

static void DestroySurface(const core::Ref<Surface*>& surface)
{
    const auto window = WindowManager::Get()->GetMainWindow();
    core::Assert::Require(logcat::Platform_RHI_Vulkan, window != nullptr, "Window must be destroyed after surface destroyed!");
    window->DestroySurface(surface);
}

static bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t cnt;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &cnt, nullptr);
    core::Array<VkExtensionProperties> extensions(cnt);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &cnt, extensions.data());
    Array my_required_extensions = {"VK_KHR_swapchain"};
    auto  filter_exts            = my_required_extensions |   //
                       remove_if([&extensions](const core::String& ext) {
                           return ranges::any_of(extensions, [&ext](const VkExtensionProperties& prop) { return ext == prop.extensionName; });
                       });
    return filter_exts.empty();
}

static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, const Surface* surface)
{
    QueueFamilyIndices indices;
    uint32_t           queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    core::Array<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
    int i = 0;
    for (const auto& queue_family: queue_families)
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
    if (!indices.IsComplete()) return false;
    if (!CheckDeviceExtensionSupport(device)) return false;
    auto support = QuerySwapChainSupportInfo(device, surface);
    if (support.formats.empty() || support.present_modes.empty()) return false;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);
    return features.samplerAnisotropy;
}

static void SelectPhysicalDevice(core::Ref<VkPhysicalDevice> physical_device, VkInstance instance, Surface* surface)
{
    PROFILE_SCOPE_AUTO;
    if (physical_devices.empty())
    {
        physical_devices = GetAvailablePhysicalDevices(instance);
    }
    for (auto candidate: physical_devices)
    {
        if (IsDeviceSuitable(candidate, surface))
        {
            physical_device = candidate;
            break;
        }
    }
    core::Assert::Require(logcat::Platform_RHI_Vulkan, physical_device != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
}

static void CreateLogicalDevice(
    VkPhysicalDevice physical_device, Surface* surface, OUT VkDevice& device, OUT VkQueue& graphics_queue, OUT VkQueue& present_queue,
    OUT VkQueue& transfer_queue
)
{
    auto indices = FindQueueFamilies(physical_device, surface);
    core::Assert::Require(logcat::Platform_RHI_Vulkan, indices.IsComplete(), "Find uncompleted queue families.");
    core::Array<VkDeviceQueueCreateInfo> queue_create_infos;
    queue_create_infos.reserve(2);
    core::Set<uint32_t> unique_queue_families = {*indices.graphics_family, *indices.present_family};
    float               queue_priority        = 1.0f;
    for (uint32_t queue_family: unique_queue_families)
    {
        VkDeviceQueueCreateInfo create_info = {};
        create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        create_info.queueFamilyIndex        = queue_family;
        create_info.queueCount              = 1;
        create_info.pQueuePriorities        = &queue_priority;
        queue_create_infos.emplace_back(create_info);
    }

    // 开启特性Dynamic Rendering
    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features = {};
    dynamic_rendering_features.sType                                    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamic_rendering_features.dynamicRendering                         = VK_TRUE;

    VkDeviceCreateInfo create_info      = {};
    create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos       = queue_create_infos.data();
    create_info.pNext                   = &dynamic_rendering_features;
    // TODO: 这里改成由配置文件配置设备特性
    auto                     cfg        = core::GetConfig<PlatformConfig>();
    VkPhysicalDeviceFeatures features   = {};
    features.samplerAnisotropy          = VK_TRUE;
    create_info.pEnabledFeatures        = &features;
    Array required_extension_cstrs      = {"VK_KHR_swapchain"};
    create_info.enabledExtensionCount   = required_extension_cstrs.size();
    create_info.ppEnabledExtensionNames = required_extension_cstrs.data();
    core::Array layers                  = {"VK_LAYER_KHRONOS_validation"};
    if (cfg->GetValidEnableValidationLayer())
    {
        create_info.enabledLayerCount   = 1;
        create_info.ppEnabledLayerNames = layers.data();
    }
    else
    {
        create_info.enabledLayerCount = 0;
    }
    VERIFY_VULKAN_RESULT(vkCreateDevice(physical_device, &create_info, nullptr, &device));
    vkGetDeviceQueue(device, *indices.graphics_family, 0, &graphics_queue);
    vkGetDeviceQueue(device, *indices.present_family, 0, &present_queue);
    vkGetDeviceQueue(device, *indices.transfer_family, 0, &transfer_queue);
}

static void DestroyLogicalDevice(core::Ref<VkDevice> device)
{
    vkDestroyDevice(*device, nullptr);
    device = nullptr;
}

static Format CreateSwapChain(
    const SwapChainSupportInfo& swapchain_support, const Surface* surface, VkPhysicalDevice physical_device, VkDevice device, ImageDesc& desc,
    core::Size2D size, OUT core::Ref<VkSwapchainKHR> swapchain
)
{
    const auto cfg            = core::GetConfig<PlatformConfig>();
    auto       surface_handle = static_cast<VkSurfaceKHR>(surface->GetNativeHandle());

    SurfaceFormat available_format{};
    for (auto& surface_format: swapchain_support.formats)
    {
        if (surface_format.format == Format::B8G8R8A8_SRGB && surface_format.color_space == ColorSpace::sRGB)
        {
            available_format = surface_format;
        }
    }
    const PresentMode present_mode = cfg->GetPresentMode();
    const uint32_t    image_count  = cfg->GetSwapchainImageCount();
    core::Assert::Require(
        logcat::Platform_RHI_Vulkan,
        image_count <= swapchain_support.max_image_count && image_count >= swapchain_support.min_image_count,
        "Unsupported swap chain image count"
    );
    VkSwapchainCreateInfoKHR swapchain_create_info{};
    swapchain_create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface          = surface_handle;
    swapchain_create_info.minImageCount    = image_count;
    swapchain_create_info.imageFormat      = RHIFormatToVkFormat(available_format.format);
    swapchain_create_info.imageColorSpace  = RHIColorSpaceToVkColorSpace(available_format.color_space);
    swapchain_create_info.imageExtent      = VkExtent2D{.width = size.width, .height = size.height};
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    desc.depth_or_layers = 1;
    desc.format          = available_format.format;
    desc.height          = size.height;
    desc.width           = size.width;
    desc.mip_levels      = 1;
    desc.initial_state   = ImageLayout::Undefined;

    QueueFamilyIndices indices                = FindQueueFamilies(physical_device, surface);
    uint32_t           queue_family_indices[] = {*indices.graphics_family, *indices.present_family};
    if (indices.graphics_family != indices.present_family)
    {
        swapchain_create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices   = queue_family_indices;
    }
    else
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    swapchain_create_info.preTransform   = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode    = RHIPresentModeToVkPresentMode(present_mode);
    swapchain_create_info.clipped        = VK_TRUE;
    VERIFY_VULKAN_RESULT(vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, swapchain.GetPtr()));
    return VkFormatToRHIFormat(swapchain_create_info.imageFormat);
}

static void DestroySwapChain(VkDevice device, core::Ref<VkSwapchainKHR> swapchain)
{
    vkDestroySwapchainKHR(device, *swapchain, nullptr);
    swapchain = nullptr;
}

GfxContext_Vulkan::GfxContext_Vulkan()
{
    post_vulkan_gfx_context_init_     = Event_GfxContextPostInitialized.AddBind(&ThisType::PostVulkanGfxContextInit);
    pre_vulkan_gfx_context_destroyed_ = Event_GfxContextPreDestroyed.AddBind(&ThisType::PreVulkanGfxContextDestroyed);
    available_layers                  = GetAvailableLayers();
    LOGGER.Info(logcat::Platform_RHI_Vulkan, "Initializing Vulkan Graphics API...");
    CreateInstance(instance_);
    CreateSurface(surface_, instance_);
    SelectPhysicalDevice(physical_device_, instance_, surface_);
    queue_family_indices_ = FindQueueFamilies(physical_device_, surface_);
    CreateLogicalDevice(physical_device_, surface_, device_, graphics_queue_, present_queue_, transfer_queue_);
    FindVulkanExtensionSymbols();
    auto cfg              = GetConfig<PlatformConfig>();
    default_color_format_ = CreateSwapChain(
        QuerySwapChainSupportInfo(), surface_, physical_device_, device_, swapchain_image_desc_, cfg->GetDefaultWindowSize(), swapchain_
    );
    Format depth_format = FindSupportedFormat(
        {Format::D32_Float, Format::D32_Float_S8X24_UInt, Format::D24_UNorm_S8_UInt},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
    core::Assert::Require(logcat::Platform_RHI_Vulkan, depth_format != Format::Count, "Failed to find supported depth format!");
    default_depth_stencil_format_ = depth_format;

    auto [name, limits] = QueryDeviceInfo();
    LOGGER.Info(logcat::Platform_RHI_Vulkan, "Using device: {}", name);
}

void GfxContext_Vulkan::ResizeSwapChain(Int32 width, Int32 height)
{
    for (auto& image_view: swapchain_image_views_)
    {
        Delete(image_view);
    }
    swapchain_image_views_.clear();
    swapchain_images_.clear();
    DestroySwapChain(device_, swapchain_);
    default_color_format_ =
        CreateSwapChain(QuerySwapChainSupportInfo(), surface_, physical_device_, device_, swapchain_image_desc_, Size2D{width, height}, swapchain_);
    uint32_t img_cnt = 0;
    VERIFY_VULKAN_RESULT(vkGetSwapchainImagesKHR(device_, swapchain_, &img_cnt, nullptr));
    auto vk_imgs = core::Array<VkImage>(img_cnt);
    VERIFY_VULKAN_RESULT(vkGetSwapchainImagesKHR(device_, swapchain_, &img_cnt, vk_imgs.data()));
    if (img_cnt <= 0)
    {
        LOGGER.Critical(logcat::Platform_RHI_Vulkan, "创建交换链获取失败");
    }
    swapchain_images_ =   //
        vk_imgs | enumerate | transform([this](const auto& pair) {
            auto& desc             = swapchain_image_desc_;
            const auto& [idx, img] = pair;
            return New<Image_Vulkan>(img, idx, desc.width, desc.height, desc.format);
        }) |
        ranges::to_vector;
    swapchain_image_views_ =   //
        swapchain_images_ | enumerate | transform([](const auto& pair) {
            const auto& [idx, img] = pair;
            ImageViewDesc desc{img};
            return New<ImageView_Vulkan>(desc);
        }) |
        ranges::to_vector;
}

SharedPtr<DescriptorSetPool> GfxContext_Vulkan::CreateDescriptorSetPool(const DescriptorSetPoolDesc& desc)
{
    return MakeShared<DescriptorSetPool_Vulkan>(desc);
}

SharedPtr<Sampler> GfxContext_Vulkan::CreateSampler(const SamplerDesc& desc, StringView debug_name)
{
    auto rtn = MakeShared<Sampler_Vulkan>(desc);
#if ELBOW_DEBUG
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
}   // namespace platform::rhi::vulkan

core::StringView VulkanErrorToString(VkResult result)
{
    switch (result)
    {
    case VK_SUCCESS: return "VK_SUCCESS";
    case VK_NOT_READY: return "VK_NOT_READY";
    case VK_TIMEOUT: return "VK_TIMEOUT";
    case VK_EVENT_SET: return "VK_EVENT_SET";
    case VK_EVENT_RESET: return "VK_EVENT_RESET";
    case VK_INCOMPLETE: return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
    default: return "Unknown error";
    }
}
