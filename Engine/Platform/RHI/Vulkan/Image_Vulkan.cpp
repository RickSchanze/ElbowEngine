/**
 * @file Image_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-25
 * @brief 
 */

#include "Image_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"
#include "Platform/RHI/GfxContext.h"
platform::rhi::vulkan::Image_Vulkan::Image_Vulkan(const ImageDesc& desc) : Image(desc)
{
    auto              ctx    = static_cast<GfxContext_Vulkan*>(GetGfxContext());
    auto              device = ctx->GetDevice();
    VkImageCreateInfo image_info{};
    image_info.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType             = RHIImageDimensionToVkImageType(desc.dimension);
    image_info.format                = RHIFormatToVkFormat(desc.format);
    image_info.extent.width          = desc.width;
    image_info.extent.height         = desc.height;
    image_info.extent.depth          = desc.depth_or_layers;
    image_info.mipLevels             = desc.mip_levels;
    image_info.arrayLayers           = desc.depth_or_layers;
    image_info.usage                 = RHIImageUsageToVkImageUsageFlags(desc.usage);
    image_info.samples               = RHISampleCountToVkSampleCount(desc.samples);
    image_info.initialLayout         = RHIImageLayoutToVkImageLayout(desc.initial_state);
    image_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices   = nullptr;
    image_info.flags                 = 0;
    image_info.tiling                = VK_IMAGE_TILING_OPTIMAL;
    VERIFY_VULKAN_RESULT(vkCreateImage(device, &image_info, nullptr, &image_handle_));
    VkMemoryRequirements mem_req{};
    vkGetImageMemoryRequirements(device, image_handle_, &mem_req);
    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize  = mem_req.size;
    alloc_info.memoryTypeIndex = GetVulkanGfxContext()->FindMemoryType(mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VERIFY_VULKAN_RESULT(vkAllocateMemory(device, &alloc_info, nullptr, &image_memory_));
    vkBindImageMemory(device, image_handle_, image_memory_, 0);
}

const char* swapchain_image_names[] = {
    "SwapChainImage0",
    "SwapChainImage1",
    "SwapChainImage2",
    "SwapChainImage3",
    "SwapChainImage4",
    "SwapChainImage5",
    "SwapChainImage6",
    "SwapChainImage7",
    "SwapChainImage8",   // 开发设备最多支持八个
};

platform::rhi::vulkan::Image_Vulkan::Image_Vulkan(
    const VkImage handle_, const int32_t index, const uint32_t width_, const uint32_t height_, const Format format_
)
{
    image_handle_         = handle_;
    desc_.width           = width_;
    desc_.height          = height_;
    desc_.format          = format_;
    desc_.usage           = IUB_SwapChain;
    desc_.depth_or_layers = 1;
    desc_.mip_levels      = 1;
    // TODO: samples应该为1吗？
    desc_.samples         = SampleCount::SC_1;
    desc_.initial_state   = ImageLayout::Undefined;
    desc_.dimension       = ImageDimension::D2;
}

platform::rhi::vulkan::Image_Vulkan::~Image_Vulkan()
{
    if (desc_.usage & IUB_SwapChain)
    {
        if (desc_.usage != IUB_SwapChain)
        {
            LOGGER.Warn(logcat::Platform_RHI_Vulkan, "Image usage IU_SwapChain cannot combined with other usage.");
        }
        else
        {
            // 交换链的Image由交换链销毁
        }
        return;
    }
    else
    {
        vkDestroyImage(GetVulkanGfxContext()->GetDevice(), image_handle_, nullptr);
        vkFreeMemory(GetVulkanGfxContext()->GetDevice(), image_memory_, nullptr);
        image_handle_ = VK_NULL_HANDLE;
        image_memory_ = VK_NULL_HANDLE;
    }
}

void platform::rhi::vulkan::Image_Vulkan::UploadData(const void* data, const size_t size)
{

}
