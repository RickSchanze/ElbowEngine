//
// Created by Echo on 2025/3/25.
//


#include "Image.hpp"
#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "Enums.hpp"
#include "GfxContext.hpp"
#include "Platform/RHI/Commands.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/ImageView.hpp"

using namespace NRHI;


Image_Vulkan::Image_Vulkan(const ImageDesc &desc) : Image(desc) {
    const auto ctx = static_cast<GfxContext_Vulkan *>(GetGfxContext());
    const auto device = ctx->GetDevice();
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = RHIImageDimensionToVkImageType(desc.Dimension);
    image_info.format = RHIFormatToVkFormat(desc.Format);
    image_info.extent.width = desc.Width;
    image_info.extent.height = desc.Height;
    image_info.extent.depth = desc.Dimension == ImageDimension::D2 ? 1 : desc.DepthOrLayers;
    image_info.mipLevels = desc.MipLevels;
    image_info.arrayLayers = desc.Dimension == ImageDimension::D2 ? desc.DepthOrLayers : 1;
    image_info.usage = RHIImageUsageToVkImageUsageFlags(desc.Usage);
    image_info.samples = RHISampleCountToVkSampleCount(desc.Samples);
    image_info.initialLayout = RHIImageLayoutToVkImageLayout(desc.InitialState);
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = nullptr;
    if (desc.DepthOrLayers == 6) {
        image_info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    VerifyVulkanResult(vkCreateImage(device, &image_info, nullptr, &image_handle_));
    VkMemoryRequirements mem_req{};
    vkGetImageMemoryRequirements(device, image_handle_, &mem_req);
    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_req.size;
    alloc_info.memoryTypeIndex = GetVulkanGfxContext()->FindMemoryType(mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VerifyVulkanResult(vkAllocateMemory(device, &alloc_info, nullptr, &image_memory_));
    vkBindImageMemory(device, image_handle_, image_memory_, 0);
    auto cmd = GfxCommandHelper::BeginSingleCommand();
    ImageSubresourceRange range{};
    if (desc_.Usage & ImageUsageBits::IUB_RenderTarget || desc_.Usage & ImageUsageBits::IUB_ShaderRead) {
        range.aspect_mask = IA_Color;
    } else if (desc_.Usage & ImageUsageBits::IUB_DepthStencil) {
        range.aspect_mask = IA_Depth;
    }
    range.base_array_layer = 0;
    range.layer_count = desc.DepthOrLayers;
    range.base_mip_level = 0;
    range.level_count = desc.MipLevels;
    auto new_layout = ImageLayout::Count;
    if (range.aspect_mask & IA_Color) {
        new_layout = ImageLayout::ShaderReadOnly;
    } else if (range.aspect_mask & IA_Depth || range.aspect_mask & IA_Stencil) {
        new_layout = ImageLayout::DepthStencilAttachment;
    }
    cmd->ImagePipelineBarrier(ImageLayout::Undefined, new_layout, this, range, AFB_None, AFB_ShaderRead, PSFB_TopOfPipe, PSFB_FragmentShader);
    cmd->Execute();
    if (image_info.mipLevels > 1) {
        GenerateMipmaps(*cmd);
    }
    GfxCommandHelper::EndSingleCommandTransfer(cmd);
}

Image_Vulkan::Image_Vulkan(VkImage handle_, Int32 index, UInt32 width_, UInt32 height_, Format format_) {
    image_handle_ = handle_;
    desc_.Width = width_;
    desc_.Height = height_;
    desc_.Format = format_;
    desc_.Usage = IUB_SwapChain;
    desc_.DepthOrLayers = 1;
    desc_.MipLevels = 1;
    // TODO: samples应该为1吗？
    desc_.Samples = SampleCount::SC_1;
    desc_.InitialState = ImageLayout::Undefined;
    desc_.Dimension = ImageDimension::D2;
}

Image_Vulkan::~Image_Vulkan() {
    if (desc_.Usage & IUB_SwapChain) {
        if (desc_.Usage != IUB_SwapChain) {
            Log(Warn) << "Image usage IU_SwapChain cannot combined with other usage.";
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

SharedPtr<Buffer> Image_Vulkan::CreateCPUVisibleBuffer() {
    const BufferDesc rtn_desc{GetWidth() * GetHeight() * GetNumChannels() * GetFormatComponentSize(), BufferUsageBits::BUB_TransferDst,
                              BMPB_HostCoherent | BMPB_HostVisible};
    auto dst_buffer = GetGfxContextRef().CreateBuffer(rtn_desc);
    // 将图像转换到TransferDst布局
    ImageSubresourceRange range{};
    range.aspect_mask = IA_Color;
    range.base_mip_level = 0;
    range.level_count = 1;
    range.base_array_layer = 0;
    range.layer_count = 1;
    const auto cmd = GfxCommandHelper::BeginSingleCommand();
    cmd->ImagePipelineBarrier(ImageLayout::ShaderReadOnly, ImageLayout::TransferSrc, this, range, AFB_ShaderRead, AFB_TransferRead,
                              PSFB_FragmentShader, PSFB_Transfer);
    cmd->CopyImageToBuffer(this, dst_buffer.get(), range, Vector3i{0, 0, 0},
                           Vector3i{static_cast<Int32>(GetWidth()), static_cast<Int32>(GetHeight()), 1});
    cmd->ImagePipelineBarrier(ImageLayout::TransferSrc, ImageLayout::ShaderReadOnly, this, range, AFB_TransferRead, AFB_ShaderRead, PSFB_Transfer,
                              PSFB_FragmentShader);
    cmd->Execute();
    GfxCommandHelper::EndSingleCommandTransfer(cmd);
    return dst_buffer;
}

UInt8 Image_Vulkan::GetFormatComponentSize() {
    const Format f = GetFormat();
    if (f == Format::Count)
        return 0;
    switch (f) {
        case Format::R8_SRGB:
        case Format::R8_UNorm:
        case Format::R8G8B8A8_UNorm:
        case Format::B8G8R8A8_UNorm:
        case Format::R8G8B8A8_SRGB:
            return 1;
        case Format::D32_Float:
        case Format::R32G32B32_Float:
        case Format::R32G32B32A32_Float:
            return 4;
        default:
            return 0;
    }
}

void Image_Vulkan::GenerateMipmaps(CommandBuffer &cmd) {
    if (desc_.MipLevels <= 1)
        return;
    // 检查支不支持线性blitting
    VkFormatProperties format_properties;
    auto &ctx = *GetVulkanGfxContext();
    vkGetPhysicalDeviceFormatProperties(ctx.GetPhysicalDevice(), RHIFormatToVkFormat(desc_.Format), &format_properties);
    if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        // TODO: Enum string
        VLOG_FATAL("图像格式 {} 不支持 linear blitting.", RHIFormatToVkFormat(desc_.Format));
    }
    UInt32 mip_width = desc_.Width;
    UInt32 mip_height = desc_.Height;
    ImageSubresourceRange range{};
    // TODO: 这里可能需要更多格式
    range.aspect_mask = IA_Color;
    range.base_array_layer = 0;
    range.layer_count = desc_.DepthOrLayers;
    range.level_count = desc_.MipLevels;
    cmd.ImagePipelineBarrier(ImageLayout::ShaderReadOnly, ImageLayout::TransferDst, this, range, AFB_ShaderRead, AFB_TransferWrite,
                             PSFB_FragmentShader, PSFB_Transfer);
    for (Int32 i = 1; i < desc_.MipLevels; i++) {
        range.base_mip_level = i - 1;
        range.level_count = 1;
        // 当前只能在Image立即创建时调用此函数, 意味着其Layout肯定是Undefined
        cmd.ImagePipelineBarrier(ImageLayout::TransferDst, ImageLayout::TransferSrc, this, range, AFB_TransferWrite, AFB_TransferRead, PSFB_Transfer,
                                 PSFB_Transfer);
        ImageBlitDesc desc{};
        desc.src_offsets[0] = {0, 0, 0};
        desc.src_offsets[1] = {static_cast<Int32>(mip_width), static_cast<Int32>(mip_height), 1};
        desc.src_subresource.aspect_mask = IA_Color;
        desc.src_subresource.base_array_layer = 0;
        desc.src_subresource.layer_count = 1;
        desc.src_subresource.base_mip_level = i - 1;
        desc.src_subresource.level_count = 1;
        desc.dst_offsets[0] = {0, 0, 0};
        desc.dst_offsets[1] = {static_cast<Int32>(mip_width > 1 ? mip_width / 2 : 1), static_cast<Int32>(mip_height > 1 ? mip_height / 2 : 1), 1};
        desc.dst_subresource.aspect_mask = IA_Color;
        desc.dst_subresource.base_array_layer = 0;
        desc.dst_subresource.layer_count = 1;
        desc.dst_subresource.base_mip_level = i;
        cmd.BlitImage(this, this, desc);
        cmd.ImagePipelineBarrier(ImageLayout::TransferSrc, ImageLayout::ShaderReadOnly, this, range, AFB_TransferRead, AFB_ShaderRead, PSFB_Transfer,
                                 PSFB_FragmentShader);
        cmd.Execute();
        mip_height = mip_height > 1 ? mip_height / 2 : 1;
        mip_width = mip_width > 1 ? mip_width / 2 : 1;
    }
    range.base_mip_level = desc_.MipLevels - 1;
    cmd.ImagePipelineBarrier(ImageLayout::TransferDst, ImageLayout::ShaderReadOnly, this, range, AFB_TransferWrite, AFB_ShaderRead, PSFB_Transfer,
                             PSFB_FragmentShader);
    cmd.Execute();
}

Sampler_Vulkan::Sampler_Vulkan(const SamplerDesc &desc) {
    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = RHIFilterToVkFilter(desc.mag);
    sampler_info.minFilter = RHIFilterToVkFilter(desc.min);
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.addressModeU = RHISamplerAddressModeToVkSamplerAddressMode(desc.u);
    sampler_info.addressModeV = RHISamplerAddressModeToVkSamplerAddressMode(desc.v);
    sampler_info.addressModeW = RHISamplerAddressModeToVkSamplerAddressMode(desc.w);
    sampler_info.anisotropyEnable = false;
    sampler_info.maxAnisotropy = desc.max_anisotropy;
    sampler_info.unnormalizedCoordinates = desc.unnormalized_coordinates;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 100.f;
    VerifyVulkanResult(vkCreateSampler(GetVulkanGfxContext()->GetDevice(), &sampler_info, nullptr, &sampler_handle_));
}

Sampler_Vulkan::~Sampler_Vulkan() { vkDestroySampler(GetVulkanGfxContext()->GetDevice(), sampler_handle_, nullptr); }

UInt32 Image::GetNumChannels() const {
    const Format f = GetFormat();
    if (f == Format::Count)
        return 0;
    switch (f) {
        case Format::D32_Float:
        case Format::R8_SRGB:
        case Format::R8_UNorm:
            return 1;
        case Format::R32G32B32_Float:
            return 3;
        case Format::R8G8B8A8_UNorm:
        case Format::B8G8R8A8_UNorm:
        case Format::R8G8B8A8_SRGB:
        case Format::R32G32B32A32_Float:
            return 4;
        default:
            return 0;
    }
}

StaticArray<SharedPtr<ImageView>, 6> Image::CreateCubemapViews() {
    if (GetDepthOrLayers() == 1)
        return {};
    StaticArray<SharedPtr<ImageView>, 6> result;
    for (Int32 i = 0; i < GetDepthOrLayers(); i++) {
        ImageViewDesc view_desc{this};
        view_desc.type = ImageDimension::D2;
        view_desc.subresource_range.base_array_layer = i;
        view_desc.subresource_range.layer_count = 1;
        result[i] = GetGfxContextRef().CreateImageView(view_desc);
    }
    return result;
}
