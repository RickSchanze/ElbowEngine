//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Misc/Other.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "IResource.hpp"

namespace rhi {
class Buffer;
struct ImageDesc {
  size_t width;
  size_t height;
  ImageUsage usage;
  ImageDimension dimension;
  UInt16 depth_or_layers; // 对于2D图像, 代表layerCount, 对于3D图像, 代表depth
  UInt16 mip_levels;
  Format format;
  SampleCount samples;
  ImageLayout initial_state; // 初始图像状态

  ImageDesc(const size_t width_, const size_t height_, const ImageUsage usage_, const Format format_,
            const ImageDimension dimension_ = ImageDimension::D2, const UInt16 depth_or_layers_ = 1, const UInt16 mip_levels_ = 1,
            const SampleCount samples_ = SampleCount::SC_1, const ImageLayout initial_state_ = ImageLayout::Undefined)
      : width(width_), height(height_), usage(usage_), dimension(dimension_), depth_or_layers(depth_or_layers_), mip_levels(mip_levels_),
        format(format_), samples(samples_), initial_state(initial_state_) {}

  /**
   * 获得一个默认构造的ImageDesc, 但是这个ImageDesc是无法使用的
   * @return
   */
  static ImageDesc Default() { return ImageDesc{0, 0, IUB_Max, Format::Count}; }
};

class Image : public IResource {
  friend class ImageView;

public:
  explicit Image(const ImageDesc &desc) : desc_(desc) {}

  [[nodiscard]] UInt64 GetWidth() const { return desc_.width; }
  [[nodiscard]] UInt64 GetHeight() const { return desc_.height; }
  [[nodiscard]] ImageUsage GetUsage() const { return desc_.usage; }
  [[nodiscard]] ImageDimension GetDimension() const { return desc_.dimension; }
  [[nodiscard]] UInt16 GetDepthOrLayers() const { return desc_.depth_or_layers; }
  [[nodiscard]] UInt16 GetMipLevels() const { return desc_.mip_levels; }
  [[nodiscard]] Format GetFormat() const { return desc_.format; }
  [[nodiscard]] SampleCount GetSampleCount() const { return desc_.samples; }
  [[nodiscard]] ImageLayout GetState() const { return desc_.initial_state; }

  [[nodiscard]] UInt32 GetNumChannels() const;

  /**
   * 创建一个CPU可见的图像并将数据拷贝进Buffer 可供保存
   * @return
   */
  [[nodiscard]] virtual SharedPtr<Buffer> CreateCPUVisibleBuffer() = 0;

protected:
  explicit Image() : desc_(0, 0, IUB_Max, Format::Count) {}

  ImageDesc desc_;
};

struct SamplerDesc {
  FilterMode mag = FilterMode::Linear; // 纹理被放大时的过滤模式
  FilterMode min = FilterMode::Linear; // 纹理被缩小时的过滤模式
  SamplerAddressMode u = SamplerAddressMode::ClampToEdge;
  SamplerAddressMode v = SamplerAddressMode::ClampToEdge;
  SamplerAddressMode w = SamplerAddressMode::Repeat;
  Bool enable_anisotropy = false;
  Float max_anisotropy = 1.0f;
  Bool unnormalized_coordinates = false;

  [[nodiscard]] size_t GetHashCode() const noexcept {
    size_t hash = 0;
    hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(static_cast<Int32>(mag)));
    hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(static_cast<Int32>(min)));
    hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(static_cast<Int32>(u)));
    hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(static_cast<Int32>(v)));
    hash = HashUtils::CombineHash(hash, std::hash<Int32>{}(static_cast<Int32>(w)));
    hash = HashUtils::CombineHash(hash, std::hash<Bool>{}(enable_anisotropy));
    hash = HashUtils::CombineHash(hash, std::hash<Float>{}(max_anisotropy));
    hash = HashUtils::CombineHash(hash, std::hash<Bool>{}(unnormalized_coordinates));
    return hash;
  }
};

class Sampler : public IResource {};

} // namespace rhi

template <> struct std::hash<rhi::SamplerDesc> {
  size_t operator()(const rhi::SamplerDesc &desc) const noexcept { return desc.GetHashCode(); }
};
