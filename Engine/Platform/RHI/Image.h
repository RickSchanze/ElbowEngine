/**
 * @file Image.h
 * @author Echo 
 * @Date 24-11-24
 * @brief 
 */

#pragma once
#include "Enums.h"
#include "IResource.h"

namespace platform::rhi
{

enum class ImageState
{
    Undefined,      // vk: UNDEFINED
    RenderTarget,   // vk: COLOR_ATTACHMENT_OPTIMAL
    DepthStencil,   // vk: DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    ShaderRead,     // vk: SHADER_READ_ONLY_OPTIMAL
    TransferSrc,    // vk: TRANSFER_SRC_OPTIMAL
    TransferDst,    // vk: TRANSFER_DST_OPTIMAL
    Present,        // vk: PRESENT_SRC_KHR
};

enum ImageUsage
{
    IU_TransferSrc  = 0b000001,   // VK_IMAGE_USAGE_TRANSFER_SRC_BIT
    IU_TransferDst  = 0b000010,   // VK_IMAGE_USAGE_TRANSFER_DST_BIT
    IU_RenderTarget = 0b000100,   // VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    IU_DepthStencil = 0b001000,   // VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
    IU_ShaderRead   = 0b010000,   // VK_IMAGE_USAGE_SAMPLED_BIT
    IU_Transient    = 0b100000,   // VK_IMAGE_USAGE_STORAGE_BIT
    IU_Max,                       // VK_IMAGE_USAGE_MAX_ENUM
};

enum class ImageDimension
{
    D_1,   // 一维图像
    D_2,   // 二维图像
    D_3,   // 三维图像
};

struct ImageDesc
{
    size_t         width;
    size_t         height;
    ImageUsage     usage;
    ImageDimension dimension;
    uint16_t       depth_or_layers;   // 对于2D图像, 代表layerCount, 对于3D图像, 代表depth
    uint16_t       mip_levels;
    Format         format;
    SampleCount    samples;
    ImageState     state;   // 初始图像状态

    ImageDesc(
        const size_t width, const size_t height, const ImageUsage usage, const Format format, const ImageDimension dimension = ImageDimension::D_2,
        const uint16_t depth_or_layers = 1, const uint16_t mipLevels = 1, const SampleCount samples = SC_1,
        const ImageState state = ImageState::Undefined
    ) :
        width(width), height(height), usage(usage), dimension(dimension), depth_or_layers(depth_or_layers), mip_levels(mipLevels), format(format),
        samples(samples), state(state)
    {
    }
};

class Image : public IResource
{
public:
    explicit Image(const ImageDesc& desc) : desc_(desc) {}

    [[nodiscard]] size_t         GetWidth() const { return desc_.width; }
    [[nodiscard]] size_t         GetHeight() const { return desc_.height; }
    [[nodiscard]] ImageUsage     GetUsage() const { return desc_.usage; }
    [[nodiscard]] ImageDimension GetDimension() const { return desc_.dimension; }
    [[nodiscard]] uint16_t       GetDepthOrLayers() const { return desc_.depth_or_layers; }
    [[nodiscard]] uint16_t       GetMipLevels() const { return desc_.mip_levels; }
    [[nodiscard]] Format         GetFormat() const { return desc_.format; }
    [[nodiscard]] SampleCount    GetSampleCount() const { return desc_.samples; }
    [[nodiscard]] ImageState     GetState() const { return desc_.state; }

protected:
    ImageDesc desc_;
};
}   // namespace platform::rhi
