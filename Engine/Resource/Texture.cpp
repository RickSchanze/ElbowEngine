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

using namespace RHI::Vulkan;

RESOURCE_NAMESPACE_BEGIN

Texture::Texture(
    Protected, const Path& InPath, const ETextureUsage InUsage,
    const SamplerInfo& SamplerInfo
) : mPath(InPath), mUsage(InUsage), mSamplerInfo(SamplerInfo)
{
    // 所有的Load操作都发生在没有注册的情况下
    // 因为只能走Create创建 这就保证了已经加载的资源不会走这个函数重新加载
    Load();
    ResourceManager::Get().RegisterResource(mPath, this);
}

Texture* Texture::Create(
    const Path& InPath, ETextureUsage InUsage, const SamplerInfo& SamplerInfo
)
{
    auto* CachedTexture = ResourceManager::Get().GetResource<Texture>(InPath);
    if (CachedTexture == nullptr)
    {
        // 指针由ResourceManager管理
        CachedTexture = new Texture(Protected{}, InPath, InUsage, SamplerInfo);
    }
    return CachedTexture;
}

Texture::~Texture()
{
    stbi_image_free(mData);
    if (mRHITexture) mRHITexture->Destroy();
    if (mRHITextureView) mRHITextureView->InternalDestroy();
    mRHITexture = nullptr;
    mData       = nullptr;
    mData       = nullptr;
}

void Texture::Load()
{
    if (!mPath.IsExist())
    {
        LOG_ERROR_CATEGORY(Resource, L"{}不存在", mPath.ToString());
        return;
    }
    const AnsiString PathStr = mPath.ToAnsiString();

    mData = stbi_load(PathStr.c_str(), &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);
    if (!mData)
    {
        LOG_ERROR_CATEGORY(Resource, L"加载纹理{}失败", mPath.ToString());
        return;
    }

    // 加载底层RHI资源
    ImageInfo TextureInfo = {};
    TextureInfo.Height = mHeight;
    TextureInfo.Width = mWidth;
    TextureInfo.ImageType = vk::ImageType::e2D;
    mRHITexture = RHI::Vulkan::Texture::CreateUnique(TextureInfo, mData);

    ImageViewInfo ViewInfo = {};
    mRHITextureView = mRHITexture->CreateImageViewUnique(ViewInfo);

    mRHISampler = Sampler::Create(mSamplerInfo);
}

TUniquePtr<RHI::Vulkan::Texture>& Texture::GetRHIResource(){
    return mRHITexture;
}

RESOURCE_NAMESPACE_END
