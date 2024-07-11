/**
 * @file Texture.cpp
 * @author Echo 
 * @Date 24-5-7
 * @brief 
 */

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "CoreGlobal.h"
#include "PlatformEvents.h"
#include "ResourceConfig.h"
#include "ResourceManager.h"
#include "stb_image.h"

using namespace RHI::Vulkan;

RESOURCE_NAMESPACE_BEGIN

Texture::Texture(
    Protected, const Path& InPath, const ETextureUsage InUsage, const SamplerInfo& SamplerInfo
) : mPath(InPath), mUsage(InUsage), mSamplerInfo(SamplerInfo)
{
    // 所有的Load操作都发生在没有注册的情况下
    // 因为只能走Create创建 这就保证了已经加载的资源不会走这个函数重新加载
    Load();
    ResourceManager::Get()->RegisterResource(mPath, this);
}

Texture* Texture::Create(const Path& InPath, ETextureUsage InUsage, const SamplerInfo& SamplerInfo)
{
    auto* CachedTexture = ResourceManager::Get()->GetResource<Texture>(InPath);
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
    TextureInfo.height    = mHeight;
    TextureInfo.width     = mWidth;
    TextureInfo.image_type = vk::ImageType::e2D;
    mRHITexture           = RHI::Vulkan::Texture::CreateUnique(TextureInfo, mData);

    ImageViewInfo ViewInfo = {};
    mRHITextureView        = mRHITexture->CreateImageViewUnique(ViewInfo);

    mRHISampler = Sampler::Create(mSamplerInfo);
}

TUniquePtr<RHI::Vulkan::Texture>& Texture::GetRHIResource()
{
    return mRHITexture;
}

RESOURCE_NAMESPACE_END

static void Load_Default_Engine_Texture_Resource(
    Texture** out_texture, ImageView** out_texture_view
)
{
    // 引擎默认纹理资产的生命周期包含整个程序运行期间
    if (gResourceConfig.bInitialized)
    {
        auto* DefaultLackTexture =
            ResourceManager::Get()->GetResource<Resource::Texture>(gResourceConfig.DefaultLackTexturePath);
        if (DefaultLackTexture)
        {
            *out_texture     = DefaultLackTexture->GetRHIResource().get();
            *out_texture_view = DefaultLackTexture->GetTextureView().get();
        }
        else
        {
            throw Exception(std::format(
                L"加载默认资产{}失败", gResourceConfig.DefaultLackTexturePath.ToString()
            ));
        }
    }
    else
    {
        throw Exception(L"加载引擎默认资产失败，请确保引擎配置文件已正确设置");
    }
}

struct RegisterOnEngineDefaultTextureLoad
{
    RegisterOnEngineDefaultTextureLoad()
    {
        Platform::OnRequestLoadDefaultLackTexture.Add(Load_Default_Engine_Texture_Resource);
    }
};

static RegisterOnEngineDefaultTextureLoad Register_On_Engine_Default_Texture_Load;
