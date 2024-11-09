/**
 * @file Image.h
 * @author Echo
 * @Date 24-4-23
 * @brief
 */

#pragma once
#include "Base/EString.h"
#include "ImageView.h"
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "vulkan/vulkan.hpp"

namespace rhi::vulkan
{
class CommandPool;

struct ImageViewInfo
{
    vk::Format           format       = vk::Format::eUndefined;            // 自动选择和Image一样的格式
    vk::ImageAspectFlags aspect_flags = vk::ImageAspectFlagBits::eColor;   // 首先选择颜色通道
    int32_t              mip_levels   = 0;                                 // 自动选择Miplevels
    const char*          name         = nullptr;
};

class ImageBase
{
public:
    ImageBase() = default;

    virtual ~ImageBase();

    // 使用一个handle来初始化此Image
    explicit ImageBase(const vk::Image& img_handle) : image_handle_(img_handle) {}

    virtual bool IsValid() const { return static_cast<bool>(image_handle_); }

    vk::Image GetHandle() const { return image_handle_; }

protected:
    vk::Image image_handle_ = VK_NULL_HANDLE;
};

// SwapChainImage不需要自己销毁 SwapChain销毁时会自动销毁
class SwapChainImage final : public ImageBase
{
public:
    explicit SwapChainImage(const vk::Image& img_handle) : ImageBase(img_handle) {}

    SwapChainImage() = default;

    ImageView* CreateImageView(const ImageViewInfo& view_info) const;

    core::SharedPtr<ImageView> CreateImageViewShared(const ImageViewInfo& view_info) const
    {
        return core::SharedPtr<ImageView>{CreateImageView(view_info)};
    }
};

struct ImageInfo
{
    vk::Format              format = vk::Format::eUndefined;
    vk::ImageUsageFlags     usage{};
    uint32_t                width           = 0;
    uint32_t                height          = 0;
    uint32_t                depth           = 1;
    uint32_t                mip_levels      = 1;
    uint32_t                array_layers    = 1;
    vk::ImageTiling         tiling          = vk::ImageTiling::eOptimal;
    vk::SampleCountFlagBits sample_count    = vk::SampleCountFlagBits::e1;
    vk::ImageType           image_type      = vk::ImageType::e2D;
    vk::MemoryPropertyFlags memory_property = vk::MemoryPropertyFlagBits::eDeviceLocal;
    vk::ImageCreateFlags    create_flags    = {};
    // 此参数代表你想转换到哪个ImageLayout 与原有的initialLayout含义不同！！！
    vk::ImageLayout         initial_layout  = vk::ImageLayout::eShaderReadOnlyOptimal;
    vk::SharingMode         sharing_mode    = vk::SharingMode::eExclusive;

    core::String name{};
    core::String debug_image_name{};
    core::String debug_image_memory_name{};

    static ImageInfo CubemapInfo(
        vk::Format format, core::StringView name = "", vk::ImageCreateFlags create_flags = vk::ImageCreateFlagBits::eCubeCompatible,
        vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, int32_t width = 0,
        int32_t height = 0, int32_t mip_level = 1, vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1,
        vk::ImageLayout initial_layout = vk::ImageLayout::eUndefined, vk::SharingMode sharing_mode = vk::SharingMode::eExclusive,
        vk::MemoryPropertyFlags memory_property = vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    ImageInfo() = default;

    explicit ImageInfo(const vk::ImageCreateInfo& image_info);
};

class Image : public ImageBase, public IRHIResource
{
protected:
    struct Protected
    {
    };

public:
    typedef ImageBase Super;

    explicit Image(ImageInfo create_info);

    static Image* Create(const ImageInfo& info);

    void Destroy() override;

    ~Image() override;

    bool IsValid() const override;
    void DeInitialize();

    int32_t    GetWidth() const { return image_info_.width; }
    int32_t    GetHeight() const { return image_info_.height; }
    int32_t    GetDepth() const { return image_info_.depth; }
    vk::Format GetFormat() const { return image_info_.format; }
    uint32_t   GetMipLevel() const { return image_info_.mip_levels; }

    /**
     * 实际为图像分配内存
     */
    virtual void Initialize();

protected:
    explicit Image() = default;

public:
    ImageView* CreateImageView(const ImageViewInfo& view_info) const;

protected:
    vk::DeviceMemory image_memory_ = nullptr;
    ImageInfo        image_info_{};
};

// TODO: 这里应该转移到RenderTextureCube
class Cubemap : public Image
{
public:
    typedef Image Super;

    enum class ECubemapFace
    {
        Up = 0,
        Down,
        Left,
        Right,
        Forward,
        Back
    };

    explicit Cubemap(const ImageInfo& image_info);

    void Initialize() override;

    ~Cubemap() override;

    void CreateCubemapImageViews(core::StringView name = "");

    ImageView* GetFaceView(ECubemapFace face);

    ImageView* GetView() const { return cubemap_image_view_; }

protected:
    core::Array<ImageView*>   cubemap_image_face_views_ = {};
    core::Array<core::String> cubemap_face_view_names_;
    ImageView*                cubemap_image_view_ = nullptr;
    core::String              cubemap_image_view_name_;
};

struct SamplerInfo
{
    vk::Filter             mag_filter               = vk::Filter::eLinear;
    vk::Filter             min_filter               = vk::Filter::eLinear;
    vk::SamplerAddressMode address_mode_u           = vk::SamplerAddressMode::eRepeat;
    vk::SamplerAddressMode address_mode_v           = vk::SamplerAddressMode::eRepeat;
    vk::SamplerAddressMode address_mode_w           = vk::SamplerAddressMode::eRepeat;
    bool                   enable_anisotropy        = true;
    float                  max_anisotropy           = 16.0f;
    vk::BorderColor        border_color             = vk::BorderColor::eIntOpaqueBlack;
    // false将纹理坐标标准化到(0,1)
    bool                   unnormalized_coordinates = false;
    bool                   enable_compare           = false;
    // Mipmap
    vk::SamplerMipmapMode  mipmap_mode              = vk::SamplerMipmapMode::eLinear;
    float                  mip_lod_bias             = 0.0f;
    float                  min_lod                  = 0.0f;
    float                  max_lod                  = 0.f;

    size_t GetHashCode() const;
};

class Texture : public Image
{
public:
    Texture(const ImageInfo& image_info, const uint8_t* data);

    Texture(const ImageInfo& img_info);

    void Initialize() override;

    static void LoadDefaultTextures();

    static Texture&   GetDefaultLackTexture();
    static ImageView& GetDefaultLackTextureView();

protected:
    static inline Texture*   s_default_lack_texture_      = nullptr;
    static inline ImageView* s_default_lack_texture_view_ = nullptr;
    static inline bool       default_textures_loaded_     = false;

    // 临时存储的变量 调用Initialize()后即置空
    const uint8_t* data_;
};

class Sampler : public IRHIResource
{
public:
    explicit Sampler(ResourceProtected, const SamplerInfo& info = {});

    static Sampler* Create(const SamplerInfo& info = {});

    static Sampler& GetDefaultSampler();

    static void DestroyAllSamplers();

    void InternalDestroy() const;

    void Destroy() override { InternalDestroy(); }

    vk::Sampler GetHandle() const { return handle_; }

    bool IsValid() const { return handle_ != nullptr; }

protected:
    vk::Sampler handle_;
    size_t      id_;

    static inline core::HashMap<size_t, Sampler*> samplers_;
};
}
