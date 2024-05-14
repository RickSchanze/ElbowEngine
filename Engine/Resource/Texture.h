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

namespace RHI::Vulkan {
class Image;
}

RESOURCE_NAMESPACE_BEGIN

enum class ETextureUsage {
    Diffuse,
};

class Texture : public IResource, public IRHIResourceContainer {
protected:
    struct Protected
    {};

public:
    Texture(Protected, const Path& InPath, ETextureUsage InUsage = ETextureUsage::Diffuse);

    static Texture* Create(const Path& InPath, ETextureUsage InUsage = ETextureUsage::Diffuse);

    bool IsValid() const override { return mData != nullptr; }

    ~Texture() override;

    int32  GetWidth() const { return mWidth; }
    int32  GetHeight() const { return mHeight; }
    int32  GetChannels() const { return mChannels; }
    uint8* GetData() const { return mData; }
    Path   GetPath() const override { return mPath; }

    void Load() final;

    RHI::Vulkan::IRHIResource* GetRHIResource() override;

protected:
    Path          mPath;
    int32         mWidth    = 0;
    int32         mHeight   = 0;
    int32         mChannels = 0;
    uint8*        mData     = nullptr;
    ETextureUsage mUsage;

    RHI::Vulkan::IRHIResource* mTextureRHIResource = nullptr;
};

RESOURCE_NAMESPACE_END
