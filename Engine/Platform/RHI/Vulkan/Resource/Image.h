/**
 * @file Image.h
 * @author Echo
 * @Date 24-4-23
 * @brief
 */

#pragma once
#include "ImageView.h"
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan
{
class CommandPool;
}

RHI_VULKAN_NAMESPACE_BEGIN

struct ImageViewInfo
{
    vk::Format           Format      = vk::Format::eUndefined;   // 自动选择和Image一样的格式
    vk::ImageAspectFlags AspectFlags = vk::ImageAspectFlagBits::eColor;   // 首先选择颜色通道
    Int32                MipLevels   = 0;                                 // 自动选择Miplevels
};

class ImageBase
{
public:
             ImageBase() = default;
    virtual ~ImageBase();

    // 使用一个hanlde来初始化此Image
    explicit ImageBase(const vk::Image& InImgHandle) : mImageHandle(InImgHandle) {}

    virtual bool IsValid() const { return static_cast<bool>(mImageHandle); }

    vk::Image GetHandle() const { return mImageHandle; }

    TSharedPtr<ImageView> CreateImageViewShared(const ImageViewInfo& InViewInfo) const;

    TUniquePtr<ImageView> CreateImageViewUnique(const ImageViewInfo& InViewInfo) const;

    virtual ImageView* CreateImageView(const ImageViewInfo& InViewInfo) const = 0;

protected:
    vk::Image mImageHandle = VK_NULL_HANDLE;
};

// SwapChainImage不需要自己销毁 SwapChain销毁时会自动销毁
class SwapChainImage final : public ImageBase
{
public:
    explicit SwapChainImage(const vk::Image& InImgHandle) : ImageBase(InImgHandle) {}

    SwapChainImage() = default;

    ImageView* CreateImageView(const ImageViewInfo& InViewInfo) const override;
};

struct ImageInfo
{
    vk::Format              Format = vk::Format::eUndefined;
    vk::ImageUsageFlags     Usage{};
    Int32                   Width          = 0;
    Int32                   Height         = 0;
    Int32                   Depth          = 1;
    UInt32                  MipLevels      = 1;
    vk::ImageTiling         Tiling         = vk::ImageTiling::eOptimal;
    vk::SampleCountFlagBits SampleCount    = vk::SampleCountFlagBits::e1;
    vk::ImageType           ImageType      = vk::ImageType::e2D;
    vk::MemoryPropertyFlags MemoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;

    ImageInfo() = default;

    explicit ImageInfo(const vk::ImageCreateInfo& InVkImageInfo);
};

class Image : public ImageBase, public IRHIResource
{
protected:
    struct Protected
    {
    };

public:
    typedef ImageBase Super;

    explicit Image(Protected, const ImageInfo& InCreateInfo);

    static TSharedPtr<Image> CreateShared(const ImageInfo& InCreateInfo);
    static TUniquePtr<Image> CreateUnique(const ImageInfo& InCreateInfo);

    void Destroy() override;

    ~Image() override;

    bool IsValid() const override;
    void Finialize();

    UInt32 GetWidth() const { return mImageInfo.Width; }
    UInt32 GetHeight() const { return mImageInfo.Height; }
    UInt32 GetDepth() const { return mImageInfo.Depth; }

protected:
    void CreateImage();

protected:
    explicit Image() = default;
    explicit Image(const ImageInfo& InImageInfo) : mImageInfo(InImageInfo) {}

public:
    ImageView* CreateImageView(const ImageViewInfo& InViewInfo) const override;

protected:
    vk::DeviceMemory mImageMemory = nullptr;
    ImageInfo        mImageInfo{};
};

struct SamplerInfo
{
    vk::Filter             MagFilter                = vk::Filter::eLinear;
    vk::Filter             MinFilter                = vk::Filter::eLinear;
    vk::SamplerAddressMode AddressModeU             = vk::SamplerAddressMode::eRepeat;
    vk::SamplerAddressMode AddressModeV             = vk::SamplerAddressMode::eRepeat;
    vk::SamplerAddressMode AddressModeW             = vk::SamplerAddressMode::eRepeat;
    bool                   bEnableAnisotropy        = true;
    Float                  MaxAnisotropy            = 16.0f;
    vk::BorderColor        BorderColor              = vk::BorderColor::eIntOpaqueBlack;
    // false将纹理坐标标准化到(0,1)
    bool                   bUnnormalizedCoordinates = false;
    bool                   bEnableCompare           = false;
    // Mipmap
    vk::SamplerMipmapMode  MipmapMode               = vk::SamplerMipmapMode::eLinear;
    Float                  MipLodBias               = 0.0f;
    Float                  MinLod                   = 0.0f;
    Float                  MaxLod                   = 0.f;

    size_t GetHashCode() const;
};

class Texture : public Image
{
public:
    static TSharedPtr<Texture> CreateShared(const ImageInfo& InImageInfo, const UInt8* InData);

    static TUniquePtr<Texture> CreateUnique(const ImageInfo& InImageInfo, const UInt8* InData);

    Texture(Protected, const ImageInfo& InImageInfo, const UInt8* InData);

    Int32 GetMipLevel() const { return mMipLevel; }

    static void LoadDefaultTextures();

    static Texture&   GetDefaultLackTexture();
    static ImageView& GetDefaultLackTextureView();

protected:
    Int32 mMipLevel;

    static inline Texture*   sDefaultLackTexture     = nullptr;
    static inline ImageView* sDefaultLackTextureView = nullptr;
    static inline bool       bDefaultTexturesLoaded  = false;
};

class Sampler : public IRHIResource
{
public:
    explicit Sampler(ResourceProtected, const SamplerInfo& InInitializer = {});

    static Sampler* Create(const SamplerInfo& InInitializer = {});

    static Sampler& GetDefaultSampler();

    static void DestroyAllSamplers();

    void InternalDestroy() const;

    void Destroy() override { InternalDestroy(); }

    vk::Sampler GetHandle() const { return mHandle; }

    bool IsValid() const { return mHandle != nullptr; }

protected:
    vk::Sampler mHandle;
    size_t      mId;

    static inline THashMap<size_t, Sampler*> sSamplers;
};

RHI_VULKAN_NAMESPACE_END
