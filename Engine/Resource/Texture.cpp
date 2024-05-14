/**
 * @file Texture.cpp
 * @author Echo 
 * @Date 24-5-7
 * @brief 
 */

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "CoreGlobal.h"
#include "ResourceManager.h"
#include "stb_image.h"

RESOURCE_NAMESPACE_BEGIN

Texture::Texture(Protected, const Path& InPath, const ETextureUsage InUsage) : mPath(InPath), mUsage(InUsage) {
    // 所有的Load操作都发生在没有注册的情况下
    // 因为只能走Create创建 这就保证了已经加载的资源不会走这个函数重新加载
    Load();
    ResourceManager::Get().RegisterResource(mPath, this);
}

Texture* Texture::Create(const Path& InPath, ETextureUsage) {
    auto* CachedTexture = ResourceManager::Get().GetResource<Texture>(InPath);
    if (CachedTexture == nullptr) {
        // 指针由ResourceManager管理
        CachedTexture = new Texture(Protected{}, InPath);
    }
    return CachedTexture;
}

Texture::~Texture() {
    stbi_image_free(mData);
    mData = nullptr;
}

void Texture::Load() {
    if (!mPath.IsExist()) {
        LOG_ERROR_CATEGORY(Resource, L"{}不存在", mPath.ToString());
    }
    const AnsiString PathStr = mPath.ToAnsiString();

    mData = stbi_load(PathStr.c_str(), &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);
    if (!mData) {
        LOG_ERROR_CATEGORY(Resource, L"加载纹理{}失败", mPath.ToString());
    }
}

RHI::Vulkan::IRHIResource* Texture::GetRHIResource() {
    return mTextureRHIResource;
}

RESOURCE_NAMESPACE_END
