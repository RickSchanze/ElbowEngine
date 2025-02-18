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
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/GfxCommandHelper.h"
#include "Platform/RHI/GfxContext.h"

using namespace platform::rhi;

vulkan::Image_Vulkan::Image_Vulkan(const ImageDesc &desc) : Image(desc) {
  auto ctx = static_cast<GfxContext_Vulkan *>(GetGfxContext());
  auto device = ctx->GetDevice();
  VkImageCreateInfo image_info{};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = RHIImageDimensionToVkImageType(desc.dimension);
  image_info.format = RHIFormatToVkFormat(desc.format);
  image_info.extent.width = desc.width;
  image_info.extent.height = desc.height;
  image_info.extent.depth = desc.depth_or_layers;
  image_info.mipLevels = desc.mip_levels;
  image_info.arrayLayers = desc.depth_or_layers;
  image_info.usage = RHIImageUsageToVkImageUsageFlags(desc.usage);
  image_info.samples = RHISampleCountToVkSampleCount(desc.samples);
  image_info.initialLayout = RHIImageLayoutToVkImageLayout(desc.initial_state);
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_info.queueFamilyIndexCount = 0;
  image_info.pQueueFamilyIndices = nullptr;
  image_info.flags = 0;
  image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  VERIFY_VULKAN_RESULT(vkCreateImage(device, &image_info, nullptr, &image_handle_));
  VkMemoryRequirements mem_req{};
  vkGetImageMemoryRequirements(device, image_handle_, &mem_req);
  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_req.size;
  alloc_info.memoryTypeIndex =
      GetVulkanGfxContext()->FindMemoryType(mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  VERIFY_VULKAN_RESULT(vkAllocateMemory(device, &alloc_info, nullptr, &image_memory_));
  vkBindImageMemory(device, image_handle_, image_memory_, 0);
}

const char *swapchain_image_names[] = {
    "SwapChainImage0", "SwapChainImage1", "SwapChainImage2", "SwapChainImage3",
    "SwapChainImage4", "SwapChainImage5", "SwapChainImage6", "SwapChainImage7",
    "SwapChainImage8", // 开发设备最多支持八个
};

vulkan::Image_Vulkan::Image_Vulkan(const VkImage handle_, const int32_t index, const uint32_t width_,
                                   const uint32_t height_, const Format format_) {
  image_handle_ = handle_;
  desc_.width = width_;
  desc_.height = height_;
  desc_.format = format_;
  desc_.usage = IUB_SwapChain;
  desc_.depth_or_layers = 1;
  desc_.mip_levels = 1;
  // TODO: samples应该为1吗？
  desc_.samples = SampleCount::SC_1;
  desc_.initial_state = ImageLayout::Undefined;
  desc_.dimension = ImageDimension::D2;
}

vulkan::Image_Vulkan::~Image_Vulkan() {
  if (desc_.usage & IUB_SwapChain) {
    if (desc_.usage != IUB_SwapChain) {
      LOGGER.Warn(logcat::Platform_RHI_Vulkan, "Image usage IU_SwapChain cannot combined with other usage.");
    } else {
      // 交换链的Image由交换链销毁
    }
    return;
  } else {
    vkDestroyImage(GetVulkanGfxContext()->GetDevice(), image_handle_, nullptr);
    vkFreeMemory(GetVulkanGfxContext()->GetDevice(), image_memory_, nullptr);
    image_handle_ = VK_NULL_HANDLE;
    image_memory_ = VK_NULL_HANDLE;
  }
}

core::SharedPtr<Buffer> vulkan::Image_Vulkan::CreateCPUVisibleBuffer() {
  BufferDesc rtn_desc{GetWidth() * GetHeight() * GetNumChannels(), BufferUsageBits::BUB_TransferDst,
                      BMPB_HostCoherent | BMPB_HostVisible};
  auto dst_buffer = GetGfxContextRef().CreateBuffer(rtn_desc);
  // 将图像转换到TransferDst布局
  ImageSubresourceRange range{};
  range.aspect_mask = IA_Color;
  range.base_mip_level = 0;
  range.level_count = 1;
  range.base_array_layer = 0;
  range.layer_count = 1;
  auto cmd = GfxCommandHelper::BeginSingleTransferCommand();
  cmd->Enqueue<Cmd_ImagePipelineBarrier>(ImageLayout::ShaderReadOnly, ImageLayout::TransferSrc, this, range,
                                         AFB_ShaderRead, AFB_TransferRead, PSFB_FragmentShader, PSFB_Transfer);
  cmd->Enqueue<Cmd_CopyImageToBuffer>(this, dst_buffer.get(), range, core::Vector3I{0, 0, 0},
                                      core::Vector3I{GetWidth(), GetHeight(), 1});
  cmd->Enqueue<Cmd_ImagePipelineBarrier>(ImageLayout::TransferSrc, ImageLayout::ShaderReadOnly, this, range,
                                         AFB_TransferRead, AFB_ShaderRead, PSFB_Transfer, PSFB_FragmentShader);
  cmd->Execute("CreateVisibleBuffer");
  GfxCommandHelper::EndSingleTransferCommand(cmd);
  return dst_buffer;
}

vulkan::Sampler_Vulkan::Sampler_Vulkan(const SamplerDesc &desc) {
  VkSamplerCreateInfo sampler_info{};
  sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_info.magFilter = RHIFilterToVkFilter(desc.mag);
  sampler_info.minFilter = RHIFilterToVkFilter(desc.min);
  sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_info.addressModeU = RHISamplerAddressModeToVkSamplerAddressMode(desc.u);
  sampler_info.addressModeV = RHISamplerAddressModeToVkSamplerAddressMode(desc.v);
  sampler_info.addressModeW = RHISamplerAddressModeToVkSamplerAddressMode(desc.w);
  sampler_info.anisotropyEnable = desc.enable_anisotropy;
  sampler_info.maxAnisotropy = desc.max_anisotropy;
  sampler_info.unnormalizedCoordinates = desc.unnormalized_coordinates;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;
  VERIFY_VULKAN_RESULT(vkCreateSampler(GetVulkanGfxContext()->GetDevice(), &sampler_info, nullptr, &sampler_handle_));
}

vulkan::Sampler_Vulkan::~Sampler_Vulkan() {
  vkDestroySampler(GetVulkanGfxContext()->GetDevice(), sampler_handle_, nullptr);
}
