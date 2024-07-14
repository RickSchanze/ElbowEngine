/**
 * @file LogicalDevice.cpp
 * @author Echo 
 * @Date 24-4-21
 * @brief 
 */

#include "LogicalDevice.h"

#include "CoreGlobal.h"
#include "RHI/Vulkan/Instance.h"
#include "RHI/Vulkan/PhysicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "SwapChain.h"
#include "vulkan/vulkan.hpp"

RHI_VULKAN_NAMESPACE_BEGIN

LogicalDevice::~LogicalDevice()
{
    Finialize();
}

TArray<vk::DescriptorSet> LogicalDevice::AllocateDescriptorSets(const vk::DescriptorSetAllocateInfo& alloc_info) const
{
    return handle_.allocateDescriptorSets(alloc_info);
}

void LogicalDevice::FreeDescriptorSets(vk::DescriptorPool descriptor_pool, const TArray<vk::DescriptorSet, std::allocator<vk::DescriptorSet>>& array)
    const
{
    handle_.freeDescriptorSets(descriptor_pool, array);
}

TUniquePtr<LogicalDevice> LogicalDevice::CreateUnique(vk::Device InDevice, const Ref<PhysicalDevice>& associated_physical_device)
{
    return MakeUnique<LogicalDevice>(ResourceProtected{}, InDevice, associated_physical_device);
}

LogicalDevice::LogicalDevice(ResourceProtected, const vk::Device InDevice, const Ref<PhysicalDevice>& associated_physical_device) :
    handle_(InDevice), associated_physical_device_(associated_physical_device)
{
    InitializePFNs();
}

void LogicalDevice::Finialize()
{
    if (!IsValid()) return;
    handle_.destroy();
    handle_ = VK_NULL_HANDLE;
}

void LogicalDevice::Destroy()
{
    Finialize();
}

TUniquePtr<SwapChain> LogicalDevice::CreateSwapChain(const uint32_t InSwapChainImageCount, int32_t InWidth, int32_t InHeight)
{
    const auto physical_device    = associated_physical_device_.get();
    const auto swap_chain_support = physical_device.QuerySwapChainSupport();
    const auto surface            = physical_device.GetAttachedInstance()->GetSurfaceHandle();

    const auto surface_format = SwapChain::ChooseSwapSurfaceFormat(swap_chain_support.formats);
    const auto present_mode   = SwapChain::ChooseSwapPresentMode(swap_chain_support.present_modes);
    const auto extent         = SwapChain::ChooseSwapExtent(swap_chain_support.capabilities, InWidth, InHeight);

    int32_t ImageCount = InSwapChainImageCount;
    if (InSwapChainImageCount == 0)
    {
        ImageCount = swap_chain_support.capabilities.minImageCount + 1;
    }
    if (swap_chain_support.capabilities.maxImageCount > 0 && ImageCount > swap_chain_support.capabilities.maxImageCount)
    {
        ImageCount = swap_chain_support.capabilities.maxImageCount;
    }
    vk::SwapchainCreateInfoKHR swap_chain_info = {};
    // clang-format off
    swap_chain_info
        .setSurface(surface)                                             // 设置表面
        .setMinImageCount(ImageCount)                                    // 设置交换链图像数量
        .setImageFormat(surface_format.format)                            // 设置图像格式
        .setImageColorSpace(surface_format.colorSpace)                    // 设置颜色空间
        .setImageExtent(extent)                                          // 设置图像大小(分辨率)
        .setImageArrayLayers(1)                                          // 每个图像包含的层次 通常是1 对于VR应用可能更多
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)         // 将图像作为颜色附着
        .setPreTransform(swap_chain_support.capabilities.currentTransform) // 指定一个固定的图像变换操作（需要交换链具有supportedTransforms特性）例如旋转90度
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)       // 指定alhpa通道是否和窗口系统中其他窗口混合，通常使用这个值忽略alpha通道
        .setPresentMode(present_mode)                                     // 设置呈现模式
        .setClipped(true)                                                // 不关心窗口系统中其他窗口遮挡的颜色
        .setOldSwapchain(VK_NULL_HANDLE);                                // 如果交换链需要重新创建，可以指定一个旧的交换链
    // clang-format on

    // 指定在多个队列族中使用交换链图像的方式
    const auto                      indicies             = physical_device.FindQueueFamilyIndices();
    const TStaticArray<uint32_t, 2> queue_family_indices = {
        indicies.graphics_family.value(),
        indicies.present_family.value(),
    };
    if (indicies.graphics_family != indicies.present_family)
    {
        swap_chain_info
            .setImageSharingMode(vk::SharingMode::eConcurrent)   // 图像可以在多个队列族使用而不需要显式改变图像所有权
            .setQueueFamilyIndices(queue_family_indices);        // 不是同一队列族时需要指定此项
    }
    else
    {
        swap_chain_info.setImageSharingMode(vk::SharingMode::eExclusive);   // 图像同一时间只能被一个队列族用于，此时无需指定FamilyIndices
    }
    graphics_queue_ = handle_.getQueue(indicies.graphics_family.value(), 0);
    present_queue_  = handle_.getQueue(indicies.present_family.value(), 0);
    return SwapChain::CreateUnique(handle_.createSwapchainKHR(swap_chain_info), this, surface_format.format, extent);
}

void LogicalDevice::CreateBuffer(
    const vk::DeviceSize InSize, const vk::BufferUsageFlags InUsage, const vk::MemoryPropertyFlags InProperties, vk::Buffer& OutBuffer,
    vk::DeviceMemory& OutBufferMemory
) const
{
    vk::BufferCreateInfo BufferInfo = {};
    BufferInfo.setSize(InSize).setUsage(InUsage).setSharingMode(vk::SharingMode::eExclusive);
    OutBuffer                              = handle_.createBuffer(BufferInfo);
    const vk::MemoryRequirements MemReq    = handle_.getBufferMemoryRequirements(OutBuffer);
    // 分配内存
    vk::MemoryAllocateInfo       AllocInfo = {};
    AllocInfo.setAllocationSize(MemReq.size).setMemoryTypeIndex(GetAssociatedPhysicalDevice().FindMemoryType(MemReq.memoryTypeBits, InProperties));
    OutBufferMemory = handle_.allocateMemory(AllocInfo);
    handle_.bindBufferMemory(OutBuffer, OutBufferMemory, 0);
}

void LogicalDevice::DestroyBuffer(const vk::Buffer buffer) const
{
    handle_.destroy(buffer);
}

void LogicalDevice::FreeMemory(const vk::DeviceMemory memory) const
{
    handle_.freeMemory(memory);
}

vk::DescriptorPool LogicalDevice::CreateDescriptorPool(const vk::DescriptorPoolCreateInfo& create_info) const
{
    return handle_.createDescriptorPool(create_info);
}

void LogicalDevice::DestroyDescriptorPool(vk::DescriptorPool pool) const
{
    handle_.destroyDescriptorPool(pool);
}

vk::DescriptorSetLayout LogicalDevice::CreateDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& create_info) const
{
    return handle_.createDescriptorSetLayout(create_info);
}

void LogicalDevice::DestroyDescriptorSetLayout(vk::DescriptorSetLayout layout) const
{
    handle_.destroyDescriptorSetLayout(layout);
}

void LogicalDevice::DestroyShaderModule(const vk::ShaderModule module) const
{
    handle_.destroyShaderModule(module);
}

vk::PipelineLayout LogicalDevice::CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& create_info) const
{
    return handle_.createPipelineLayout(create_info);
}

void LogicalDevice::UpdateDescriptorSets(
    const vk::ArrayProxy<const vk::WriteDescriptorSet>& descriptor_writes, const vk::ArrayProxy<vk::CopyDescriptorSet>& descriptor_copies
) const
{
    handle_.updateDescriptorSets(descriptor_writes, descriptor_copies);
}

vk::Result LogicalDevice::MapMemory(const vk::DeviceMemory InMemory, const vk::DeviceSize InSize, const vk::DeviceSize InOffset, void** OutData) const
{
    return handle_.mapMemory(InMemory, InOffset, InSize, vk::MemoryMapFlags(), OutData);
}

void LogicalDevice::UnmapMemory(const vk::DeviceMemory InMemory) const
{
    handle_.unmapMemory(InMemory);
}

vk::Result LogicalDevice::WaitForFences(vk::ArrayProxy<vk::Fence> fences, bool wait_all, uint64_t timeout) const
{
    return handle_.waitForFences(fences, wait_all, timeout);
}

void LogicalDevice::ResetFences(const vk::ArrayProxy<vk::Fence> fences) const
{
    handle_.resetFences(fences);
}

TArray<vk::CommandBuffer> LogicalDevice::AllocateCommandBuffers(const vk::CommandBufferAllocateInfo& allocate_info) const
{
    return handle_.allocateCommandBuffers(allocate_info);
}

void LogicalDevice::DestroySampler(vk::Sampler sampler) const
{
    handle_.destroySampler(sampler);
}

void LogicalDevice::DestroySwapChain(const vk::SwapchainKHR swap_chain) const
{
    handle_.destroySwapchainKHR(swap_chain);
}

vk::Semaphore LogicalDevice::CreateDeviceSemaphore(const vk::SemaphoreCreateInfo& create_info) const
{
    return handle_.createSemaphore(create_info);
}

void LogicalDevice::DestroySemaphore(const vk::Semaphore semaphore) const
{
    handle_.destroySemaphore(semaphore);
}

vk::Fence LogicalDevice::CreateFence(const vk::FenceCreateInfo& create_info) const
{
    return handle_.createFence(create_info);
}

void LogicalDevice::DestroyFence(const vk::Fence fence) const
{
    handle_.destroyFence(fence);
}

vk::RenderPass LogicalDevice::CreateRenderPass(const vk::RenderPassCreateInfo& create_info) const
{
    return handle_.createRenderPass(create_info);
}

void LogicalDevice::DestroyRenderPass(vk::RenderPass render_pass) const
{
    handle_.destroyRenderPass(render_pass);
}

void LogicalDevice::SetObjectDebugName(const vk::DebugUtilsObjectNameInfoEXT& name_info) const
{
#ifdef ELBOW_DEBUG
    const VkDebugUtilsObjectNameInfoEXT name_info_ext = name_info;
    vkSetDebugUtilsObjectNameEXT_(handle_, &name_info_ext);
#endif
}

#define SET_DEBUG_NAME_BODY(obj_type_enum, obj_type)                                    \
    vk::DebugUtilsObjectNameInfoEXT name_info;                                          \
    name_info.pObjectName  = name;                                                      \
    name_info.objectType   = vk::ObjectType::obj_type_enum;                             \
    name_info.objectHandle = reinterpret_cast<uint64_t>(static_cast<obj_type>(handle)); \
    SetObjectDebugName(name_info);

void LogicalDevice::SetCommandBufferDebugName(const vk::CommandBuffer handle, const char* name) const
{
#ifdef ELBOW_DEBUG
    SET_DEBUG_NAME_BODY(eCommandBuffer, VkCommandBuffer);
#endif
}

void LogicalDevice::SetImageDebugName(const vk::Image handle, const char* name) const
{
#ifdef ELBOW_DEBUG
    SET_DEBUG_NAME_BODY(eImage, VkImage);
#endif
}

void LogicalDevice::SetImageViewDebugName(const vk::ImageView handle, const char* name) const
{
#if ELBOW_DEBUG
    SET_DEBUG_NAME_BODY(eImageView, VkImageView)
#endif
}

void LogicalDevice::SetRenderPassDebugName(const vk::RenderPass handle, const char* name) const
{
#ifdef ELBOW_DEBUG
    SET_DEBUG_NAME_BODY(eRenderPass, VkRenderPass)
#endif
}

void LogicalDevice::SetFramebufferDebugName(const vk::Framebuffer handle, const char* name) const
{
#ifdef ELBOW_DEBUG
    SET_DEBUG_NAME_BODY(eFramebuffer, VkFramebuffer)
#endif
}

void LogicalDevice::SetShaderModuleDebugName(const vk::ShaderModule handle, const char* name) const
{
#ifdef ELBOW_DEBUG
    SET_DEBUG_NAME_BODY(eShaderModule, VkShaderModule)
#endif
}

void LogicalDevice::SetBufferDebugName(vk::Buffer handle, const char* name) const
{
#ifdef ELBOW_DEBUG
    SET_DEBUG_NAME_BODY(eBuffer, VkBuffer)
#endif
}

void LogicalDevice::SetBufferMemoryDebugName(vk::DeviceMemory handle, const char* name) const
{
#ifdef ELBOW_DEBUG
    SET_DEBUG_NAME_BODY(eDeviceMemory, VkDeviceMemory)
#endif
}

void LogicalDevice::InitializePFNs(){
    vkSetDebugUtilsObjectNameEXT_ = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(VulkanContext::Get()->GetVulkanInstance()->GetHandle(), "vkSetDebugUtilsObjectNameEXT"));
}

RHI_VULKAN_NAMESPACE_END
