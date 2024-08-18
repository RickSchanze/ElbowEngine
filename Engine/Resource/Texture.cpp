/**
 * @file Texture.cpp
 * @author Echo 
 * @Date 24-5-7
 * @brief 
 */

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "CoreGlobal.h"
#include "PlatformEvents.h"
#include "ResourceConfig.h"
#include "ResourceManager.h"
#include "RHI/Vulkan/VulkanContext.h"

#include "tinyexr.h"

using namespace RHI::Vulkan;

RESOURCE_NAMESPACE_BEGIN

Texture::Texture(Protected, const Path& path, const ETextureUsage usage, const SamplerInfo& sampler_info) :
    path_(path), usage_(usage), sampler_info_(sampler_info)
{
    // 所有的Load操作都发生在没有注册的情况下
    // 因为只能走Create创建 这就保证了已经加载的资源不会走这个函数重新加载
    Load();
    ResourceManager::Get()->RegisterResource(path_, this);
}

Texture* Texture::Create(const Path& path, ETextureUsage usage, const SamplerInfo& sampler_info)
{
    auto* cached_texture = ResourceManager::Get()->GetResource<Texture>(path);
    if (cached_texture == nullptr)
    {
        // 指针由ResourceManager管理
        cached_texture = new Texture(Protected{}, path, usage, sampler_info);
    }
    return cached_texture;
}

Texture::~Texture()
{
    stbi_image_free(data_);
    if (rhi_texture_) rhi_texture_->Destroy();
    if (rhi_texture_view_) rhi_texture_view_->InternalDestroy();
    rhi_texture_ = nullptr;
    data_        = nullptr;
    data_        = nullptr;
}

void Texture::Load()
{
    if (!path_.IsExist())
    {
        LOG_ERROR_CATEGORY(Resource, L"{}不存在", path_.ToAbsoluteString());
        return;
    }
    const AnsiString PathStr = path_.ToAbsoluteAnsiString();

    data_ = stbi_load(PathStr.c_str(), &width_, &height_, &channels_, STBI_rgb_alpha);
    if (!data_)
    {
        LOG_ERROR_CATEGORY(Resource, L"加载纹理{}失败", path_.ToAbsoluteString());
        return;
    }

    // 加载底层RHI资源
    ImageInfo texture_info  = {};
    texture_info.height     = height_;
    texture_info.width      = width_;
    texture_info.image_type = vk::ImageType::e2D;
    texture_info.usage      = vk::ImageUsageFlagBits::eSampled;
    texture_info.name = path_.ToRelativeAnsiString();
    rhi_texture_            = new RHI::Vulkan::Texture(texture_info, data_);

    ImageViewInfo view_info = {};
    rhi_texture_view_       = rhi_texture_->CreateImageView(view_info);

    rhi_sampler_ = Sampler::Create(sampler_info_);
}

RHI::Vulkan::Texture*Texture::GetRHIResource()
{
    return rhi_texture_;
}

Texture* Texture::GetDefaultLackTexture()
{
    return Create(gResourceConfig.default_lack_texture_path);
}

RESOURCE_NAMESPACE_END

static void Load_Default_Engine_Texture_Resource(Texture** out_texture, ImageView** out_texture_view)
{
    // 引擎默认纹理资产的生命周期包含整个程序运行期间
    if (gResourceConfig.bInitialized)
    {
        auto* DefaultLackTexture = Resource::Texture::Create(gResourceConfig.default_lack_texture_path);
        if (DefaultLackTexture)
        {
            *out_texture                                          = DefaultLackTexture->GetRHIResource();
            *out_texture_view                                     = DefaultLackTexture->GetTextureView();
            static const char* default_lack_image_debug_name      = "DefaultLackImage";
            static const char* default_lack_image_debug_view_name = "DefaultLackImageView";
            VulkanContext::Get()->GetLogicalDevice()->SetImageDebugName(
                DefaultLackTexture->GetRHIResource()->GetHandle(), default_lack_image_debug_name
            );
            VulkanContext::Get()->GetLogicalDevice()->SetImageViewDebugName(
                DefaultLackTexture->GetTextureView()->GetHandle(), default_lack_image_debug_view_name
            );
        }
        else
        {
            throw Exception(std::format(L"加载默认资产{}失败", gResourceConfig.default_lack_texture_path.ToAbsoluteString()));
        }
    }
    else
    {
        throw Exception(L"加载引擎默认资产失败，请确保引擎配置文件已正确设置");
    }
}

struct RegisterOnEngineDefaultTextureLoad
{
    RegisterOnEngineDefaultTextureLoad() { Platform::OnRequestLoadDefaultLackTexture.Add(Load_Default_Engine_Texture_Resource);
    }
};

static RegisterOnEngineDefaultTextureLoad Register_On_Engine_Default_Texture_Load;
