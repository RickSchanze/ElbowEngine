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
    Texture(
        Protected, const Path& InPath, ETextureUsage InUsage,
        const RHI::Vulkan::SamplerInfo& SamplerInfo
    );

    static Texture* Create(
        const Path& InPath, ETextureUsage InUsage = ETextureUsage::Diffuse,
        const RHI::Vulkan::SamplerInfo& SamplerInfo = {}
    );

    bool IsValid() const override { return mData != nullptr; }

    ~Texture() override;

    Int32                 GetWidth() const { return mWidth; }
    Int32                 GetHeight() const { return mHeight; }
    Int32                 GetChannels() const { return mChannels; }
    UInt8*                GetData() const { return mData; }
    Path                  GetPath() const override { return mPath; }
    RHI::Vulkan::Sampler* GetSampler() const { return mRHISampler; }

    void Load() final;

    TUniquePtr<RHI::Vulkan::Texture>& GetRHIResource() override;

protected:
    Path          mPath;
    Int32         mWidth    = 0;
    Int32         mHeight   = 0;
    Int32         mChannels = 0;
    UInt8*        mData     = nullptr;
    ETextureUsage mUsage;

private:
    TUniquePtr<RHI::Vulkan::Texture>   mRHITexture     = nullptr;
    TUniquePtr<RHI::Vulkan::ImageView> mRHITextureView = nullptr;
    RHI::Vulkan::Sampler*              mRHISampler     = nullptr;

    RHI::Vulkan::SamplerInfo mSamplerInfo;
};

RESOURCE_NAMESPACE_END
