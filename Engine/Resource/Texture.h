/**
 * @file Texture.h
 * @author Echo 
 * @Date 24-5-7
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Interface/IResource.h"
#include "Interface/IRHIResourceContainer.h"
#include "Path/Path.h"
#include "ResourceCommon.h"
#include "RHI/Vulkan/Resource/Image.h"

namespace RHI::Vulkan
{
class Texture;
class Image;
class Sampler;
class ImageView;
}   // namespace RHI::Vulkan

RESOURCE_NAMESPACE_BEGIN

enum class ETextureUsage
{
    Diffuse,
};

class Texture : public IResource, public IRHIResourceContainer<RHI::Vulkan::Texture>
{
    friend class RHI::Vulkan::Texture;

protected:
    struct Protected
    {
    };

public:
    Texture(Protected, const Path& path, ETextureUsage usage, const RHI::Vulkan::SamplerInfo& sampler_info = {});

    static Texture* Create(const Path& path, ETextureUsage usage = ETextureUsage::Diffuse, const RHI::Vulkan::SamplerInfo& sampler_info = {});

    bool IsValid() const override { return data_ != nullptr; }

    ~Texture() override;

    int32_t  GetWidth() const { return width_; }
    int32_t  GetHeight() const { return height_; }
    int32_t  GetChannels() const { return channels_; }
    uint8_t* GetData() const { return data_; }
    Path     GetPath() const override { return path_; }

    RHI::Vulkan::Sampler*   GetSampler() const { return rhi_sampler_; }
    RHI::Vulkan::ImageView* GetTextureView() const { return rhi_texture_view_; }

    void Load() final;

    RHI::Vulkan::Texture* GetRHIResource() override;

    static Texture* GetDefaultLackTexture();

protected:
    Path          path_;
    int32_t       width_    = 0;
    int32_t       height_   = 0;
    int32_t       channels_ = 0;
    uint8_t*      data_     = nullptr;
    ETextureUsage usage_;

private:
    RHI::Vulkan::Texture*   rhi_texture_      = nullptr;
    RHI::Vulkan::ImageView* rhi_texture_view_ = nullptr;
    RHI::Vulkan::Sampler*   rhi_sampler_      = nullptr;

    RHI::Vulkan::SamplerInfo sampler_info_;
};

RESOURCE_NAMESPACE_END
