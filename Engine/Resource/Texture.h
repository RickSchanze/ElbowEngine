/**
 * @file Texture.h
 * @author Echo 
 * @Date 24-5-7
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Path/Path.h"
#include "ResourceCommon.h"

namespace RHI::Vulkan {
class Image;
}

RESOURCE_NAMESPACE_BEGIN

class Texture {
protected:
    struct Protected
    {};

public:
    Texture(Protected, const Path& InPath) : mPath(InPath) { LoadTexture(); }

    static SharedPtr<Texture> CreateShared(const Path& InPath) { return std::make_shared<Texture>(Protected{}, InPath); }

    bool IsValid() const { return mData != nullptr; }

    ~Texture();

    int32  GetWidth() const { return mWidth; }
    int32  GetHeight() const { return mHeight; }
    int32  GetChannels() const { return mChannels; }
    uint8* GetData() const { return mData; }
    Path   GetPath() const { return mPath; }

protected:
    void LoadTexture();

protected:
    Path   mPath;
    int32  mWidth    = 0;
    int32  mHeight   = 0;
    int32  mChannels = 0;
    uint8* mData     = nullptr;
};

RESOURCE_NAMESPACE_END
