/**
 * @file Texture.h
 * @author Echo 
 * @Date 24-5-7
 * @brief
 * TODO: RenderTexture
 */

#pragma once
#include "CoreDef.h"
#include "Interface/IResource.h"
#include "Interface/IRHIResourceContainer.h"
#include "Path/Path.h"
#include "RHI/Vulkan/Resource/Image.h"

namespace rhi::vulkan
{
class Texture;
class Image;
class Sampler;
class ImageView;
}   // namespace rhi::vulkan

namespace res
{
enum class ETextureUsage
{
    None,
    Diffuse,
    Skybox2D,
    SkyboxCube,
    SkyboxFace,
};

/**
 * 纹理资源类
 * 现在默认无法write/read(即创建就释放CPU侧资源)
 * TODO: 实现真正意义上的"RHI"
 */
class Texture : public IResource, public IRHIResourceContainer<rhi::vulkan::Texture>
{
    friend class rhi::vulkan::Texture;

public:
    Texture() = default;

    static Texture* Create(
        const Path& path, ETextureUsage usage = ETextureUsage::Diffuse, const rhi::vulkan::SamplerInfo& sampler_info = {},
        vk::ImageLayout init_transition = vk::ImageLayout::eShaderReadOnlyOptimal
    );

    bool IsValid() const override { return rhi_texture_ != nullptr; }

    ~Texture() override;

    int32_t       GetWidth() const { return width_; }
    int32_t       GetHeight() const { return height_; }
    int32_t       GetChannels() const { return channels_; }
    uint8_t*      GetData() const { return data_; }
    Path          GetPath() const override { return path_; }
    ETextureUsage GetUsage() const { return usage_; }

    bool IsDefaultLackTexture() const { return this == GetDefaultLackTexture(); }

    rhi::vulkan::Sampler*   GetSampler() const { return rhi_sampler_; }
    rhi::vulkan::ImageView* GetTextureView() const { return rhi_texture_view_; }

    void Load() override;

    /**
     * 释放纹理在CPU侧的资源
     */
    virtual void ReleaseMemoryCPU();

    /**
     * CPU侧纹理资源是否有效
     * @return
     */
    virtual bool IsCPUMemoryAvailable();

    rhi::vulkan::Texture* GetRHIResource() override;

    static Texture* GetDefaultLackTexture();

    vk::Image  GetLowLevelImage() const;
    vk::Format GetLowLevelFormat() const;

    Texture(
        const Path& path, ETextureUsage usage, const rhi::vulkan::SamplerInfo& sampler_info = {},
        vk::ImageLayout init_transition_to_layout = vk::ImageLayout::eShaderReadOnlyOptimal
    );

protected:
    Path          path_;
    int32_t       width_    = 0;
    int32_t       height_   = 0;
    int32_t       channels_ = 0;
    uint8_t*      data_     = nullptr;
    ETextureUsage usage_    = ETextureUsage::None;

    rhi::vulkan::Texture*   rhi_texture_      = nullptr;
    rhi::vulkan::ImageView* rhi_texture_view_ = nullptr;
    rhi::vulkan::Sampler*   rhi_sampler_      = nullptr;

    rhi::vulkan::SamplerInfo sampler_info_;

private:
    // 一开始要转换到什么Layout
    vk::ImageLayout init_transition_to_layout_;
};

class TextureCube : public Texture
{
public:
    /**
     * 创建一个TextureCube
     * @param cube_folder 立方体贴图文件夹 应该包含以_Left,_Right,_Top,_Bottom,_Front,_Back结尾的6张图片 没有包含则会使用DefaultLackTexture并警告
     * @param sampler_info
     * @return
     */
    static TextureCube* Create(const Path& cube_folder, const rhi::vulkan::SamplerInfo& sampler_info = {});

    explicit TextureCube(const Path& cube_folder, const rhi::vulkan::SamplerInfo& sampler_info = {});

    void Load() override;

    // 创建的Texture*由ResourceManager管理
    ~TextureCube() override;

    rhi::vulkan::ImageView* GetFaceView(int face) const { return views_[face]; }

protected:
    TStaticArray<rhi::vulkan::ImageView*, 6> views_{};
    TStaticArray<AnsiString, 6> view_names_{};
};
}
