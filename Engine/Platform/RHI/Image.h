/**
 * @file Image.h
 * @author Echo 
 * @Date 24-11-24
 * @brief 
 */

#pragma once
#include "Core/Math/MathTypes.h"
#include "Enums.h"
#include "IResource.h"

namespace platform::rhi
{
class ImageView;
}
namespace platform::rhi
{
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
    ImageLayout    initial_state;   // 初始图像状态

    ImageDesc(
        const size_t width_, const size_t height_, const ImageUsage usage_, const Format format_,
        const ImageDimension dimension_ = ImageDimension::D2, const uint16_t depth_or_layers_ = 1, const uint16_t mip_levels_ = 1,
        const SampleCount samples_ = SampleCount::SC_1, const ImageLayout initial_state_ = ImageLayout::Undefined
    ) :
        width(width_), height(height_), usage(usage_), dimension(dimension_), depth_or_layers(depth_or_layers_), mip_levels(mip_levels_),
        format(format_), samples(samples_), initial_state(initial_state_)
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

    [[nodiscard]] size_t         GetWidth() const { return desc_.width; }
    [[nodiscard]] size_t         GetHeight() const { return desc_.height; }
    [[nodiscard]] ImageUsage     GetUsage() const { return desc_.usage; }
    [[nodiscard]] ImageDimension GetDimension() const { return desc_.dimension; }
    [[nodiscard]] uint16_t       GetDepthOrLayers() const { return desc_.depth_or_layers; }
    [[nodiscard]] uint16_t       GetMipLevels() const { return desc_.mip_levels; }
    [[nodiscard]] Format         GetFormat() const { return desc_.format; }
    [[nodiscard]] SampleCount    GetSampleCount() const { return desc_.samples; }
    [[nodiscard]] ImageLayout    GetState() const { return desc_.initial_state; }

    virtual void UploadData(const void* data, const size_t size) = 0;

protected:
    explicit Image() : desc_(0, 0, IUB_Max, Format::Count) {}

    ImageDesc desc_;
};

struct SamplerDesc
{
    FilterMode         mag                      = FilterMode::Linear;   // 纹理被放大时的过滤模式
    FilterMode         min                      = FilterMode::Linear;   // 纹理被缩小时的过滤模式
    SamplerAddressMode u                        = SamplerAddressMode::Repeat;
    SamplerAddressMode v                        = SamplerAddressMode::Repeat;
    SamplerAddressMode w                        = SamplerAddressMode::Repeat;
    Bool               enable_anisotropy        = false;
    Float              max_anisotropy           = 1.0f;
    Bool               unnormalized_coordinates = false;

    size_t GetHashCode() const noexcept;
};

class Sampler : public IResource
{
};

}   // namespace platform::rhi
template<>
struct std::hash<platform::rhi::SamplerDesc>
{
    size_t operator()(const platform::rhi::SamplerDesc& desc) const noexcept;
};
