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
class ImageView;
}
namespace platform::rhi
{

enum ImageUsage
{
    IU_TransferSrc  = 0b0000001,   // VK_IMAGE_USAGE_TRANSFER_SRC_BIT
    IU_TransferDst  = 0b0000010,   // VK_IMAGE_USAGE_TRANSFER_DST_BIT
    IU_RenderTarget = 0b0000100,   // VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    IU_DepthStencil = 0b0001000,   // VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
    IU_ShaderRead   = 0b0010000,   // VK_IMAGE_USAGE_SAMPLED_BIT
    IU_Transient    = 0b0100000,   // VK_IMAGE_USAGE_STORAGE_BIT
    IU_SwapChain    = 0b1000000,   // For swapchain use
    IU_Max,                        // VK_IMAGE_USAGE_MAX_ENUM
};

enum class ImageDimension
{
    D1,   // 一维图像
    D2,   // 二维图像
    D3,   // 三维图像
    Cube,
    Array1D,
    Array2D,
    ArrayCube,
    Count,
};

struct ImageDesc
{
    size_t           width;
    size_t           height;
    ImageUsage       usage;
    ImageDimension   dimension;
    uint16_t         depth_or_layers;   // 对于2D图像, 代表layerCount, 对于3D图像, 代表depth
    uint16_t         mip_levels;
    Format           format;
    SampleCountBits  samples;
    ImageLayout      initial_state;   // 初始图像状态
    core::StringView name;

    ImageDesc(
        const core::StringView name_, const size_t width_, const size_t height_, const ImageUsage usage_, const Format format_,
        const ImageDimension dimension_ = ImageDimension::D2, const uint16_t depth_or_layers_ = 1, const uint16_t mip_levels_ = 1,
        const SampleCountBits samples_ = SC_1, const ImageLayout initial_state_ = ImageLayout::Undefined
    ) :
        width(width_), height(height_), usage(usage_), dimension(dimension_), depth_or_layers(depth_or_layers_), mip_levels(mip_levels_),
        format(format_), samples(samples_), initial_state(initial_state_), name(name_)
    {
    }

    /**
     * 获得一个默认构造的ImageDesc, 但是这个ImageDesc是无法使用的
     * @return
     */
    static ImageDesc Default();
};

class Image : public IResource
{
    friend class ImageView;

public:
    explicit Image(const ImageDesc& desc) : desc_(desc) {}

    [[nodiscard]] size_t          GetWidth() const { return desc_.width; }
    [[nodiscard]] size_t          GetHeight() const { return desc_.height; }
    [[nodiscard]] ImageUsage      GetUsage() const { return desc_.usage; }
    [[nodiscard]] ImageDimension  GetDimension() const { return desc_.dimension; }
    [[nodiscard]] uint16_t        GetDepthOrLayers() const { return desc_.depth_or_layers; }
    [[nodiscard]] uint16_t        GetMipLevels() const { return desc_.mip_levels; }
    [[nodiscard]] Format          GetFormat() const { return desc_.format; }
    [[nodiscard]] SampleCountBits GetSampleCount() const { return desc_.samples; }
    [[nodiscard]] ImageLayout     GetState() const { return desc_.initial_state; }

protected:
    explicit Image() : desc_("", 0, 0, IU_Max, Format::Count) {}

    ImageDesc desc_;
};
}   // namespace platform::rhi
