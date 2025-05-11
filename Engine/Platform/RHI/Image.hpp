//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Core/Collection/StaticArray.hpp"
#include "Core/Core.hpp"
#include "Core/Misc/Other.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "IResource.hpp"

namespace NRHI {
    class Buffer;
    struct ImageDesc {
        UInt32 Width;
        UInt32 Height;
        ImageUsage Usage;
        ImageDimension Dimension;
        UInt16 DepthOrLayers; // 对于2D图像, 代表layerCount, 对于3D图像, 代表depth
        UInt16 MipLevels;
        Format Format;
        SampleCount Samples;
        ImageLayout InitialState; // 初始图像状态

        ImageDesc(const UInt32 width_, const UInt32 height_, const ImageUsage usage_, const ::NRHI::Format format_,
                  const ImageDimension dimension_ = ImageDimension::D2, const Int32 depth_or_layers_ = 1, const UInt16 mip_levels_ = 1,
                  const SampleCount samples_ = SampleCount::SC_1, const ImageLayout initial_state_ = ImageLayout::Undefined) :
            Width(width_), Height(height_), Usage(usage_), Dimension(dimension_), DepthOrLayers(depth_or_layers_), MipLevels(mip_levels_),
            Format(format_), Samples(samples_), InitialState(initial_state_) {}

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

        UInt64 GetWidth() const { return desc_.Width; }
        UInt64 GetHeight() const { return desc_.Height; }
        ImageUsage GetUsage() const { return desc_.Usage; }
        ImageDimension GetDimension() const { return desc_.Dimension; }
        UInt16 GetDepthOrLayers() const { return desc_.DepthOrLayers; }
        UInt16 GetMipLevelCount() const { return desc_.MipLevels; }
        Format GetFormat() const { return desc_.Format; }
        SampleCount GetSampleCount() const { return desc_.Samples; }
        ImageLayout GetState() const { return desc_.InitialState; }

        UInt32 GetNumChannels() const;

        /**
         * 创建一个CPU可见的图像并将数据拷贝进Buffer 可供保存
         * @return
         */
        virtual SharedPtr<Buffer> CreateCPUVisibleBuffer() = 0;

        StaticArray<SharedPtr<ImageView>, 6> CreateCubemapViews();

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

        size_t GetHashCode() const noexcept {
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

template<>
struct std::hash<NRHI::SamplerDesc> {
    size_t operator()(const NRHI::SamplerDesc &desc) const noexcept { return desc.GetHashCode(); }
};
