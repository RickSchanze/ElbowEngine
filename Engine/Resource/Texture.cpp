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

#include "RHI/Vulkan/Render/CommandPool.h"
#include "RHI/Vulkan/Resource/Buffer.h"
#include "tinyexr.h"
#include "Utils/PathUtils.h"

using namespace RHI::Vulkan;

RESOURCE_NAMESPACE_BEGIN

Texture::Texture(const Path& path, const ETextureUsage usage, const SamplerInfo& sampler_info, const vk::ImageLayout t) :
    path_(path), usage_(usage), sampler_info_(sampler_info), init_transition_to_layout_(t)
{
}

Texture* Texture::Create(const Path& path, ETextureUsage usage, const SamplerInfo& sampler_info, vk::ImageLayout init_transition)
{
    // 所有的Load操作都发生在没有注册的情况下
    // 因为只能走Create创建 这就保证了已经加载的资源不会走这个函数重新加载
    auto* cached_texture = ResourceManager::Get()->GetResource<Texture>(path);
    if (cached_texture == nullptr)
    {
        // 指针由ResourceManager管理
        cached_texture = new Texture(path, usage, sampler_info, init_transition);
        cached_texture->Load();
        ResourceManager::Get()->RegisterResource(path, cached_texture);
        cached_texture->ReleaseMemoryCPU();
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
    ImageInfo texture_info      = {};
    texture_info.height         = height_;
    texture_info.width          = width_;
    texture_info.image_type     = vk::ImageType::e2D;
    texture_info.usage          = vk::ImageUsageFlagBits::eSampled;
    texture_info.name           = path_.ToRelativeAnsiString();
    texture_info.initial_layout = init_transition_to_layout_;
    rhi_texture_                = new RHI::Vulkan::Texture(texture_info, data_);
    rhi_texture_->Initialize();

    ImageViewInfo view_info = {};
    rhi_texture_view_       = rhi_texture_->CreateImageView(view_info);

    rhi_sampler_ = Sampler::Create(sampler_info_);
}

void Texture::ReleaseMemoryCPU()
{
    if (data_)
    {
        STBI_FREE(data_);
        data_ = nullptr;
    }
}

bool Texture::IsCPUMemoryAvailable()
{
    return data_ != nullptr;
}

RHI::Vulkan::Texture* Texture::GetRHIResource()
{
    return rhi_texture_;
}

Texture* Texture::GetDefaultLackTexture()
{
    return Create(gResourceConfig.default_lack_texture_path);
}

vk::Image Texture::GetLowlevelImage() const
{
    return rhi_texture_->GetHandle();
}

vk::Format Texture::GetLowlevelFormat() const
{
    return rhi_texture_->GetFormat();
}

TextureCube* TextureCube::Create(const Path& cube_folder, const RHI::Vulkan::SamplerInfo& sampler_info)
{
    // 所有的Load操作都发生在没有注册的情况下
    // 因为只能走Create创建 这就保证了已经加载的资源不会走这个函数重新加载
    auto* cached_texture = ResourceManager::Get()->GetResource<TextureCube>(cube_folder);
    if (cached_texture == nullptr)
    {
        // 指针由ResourceManager管理
        cached_texture = new TextureCube(cube_folder, sampler_info);
        cached_texture->Load();
        ResourceManager::Get()->RegisterResource(cube_folder, cached_texture);
        cached_texture->ReleaseMemoryCPU();
    }
    return cached_texture;
}

TextureCube::TextureCube(const Path& cube_folder, const RHI::Vulkan::SamplerInfo& sampler_info)
{
    path_         = cube_folder;
    sampler_info_ = sampler_info;
    usage_        = ETextureUsage::SkyboxCube;
}

void TextureCube::Load()
{
    // 找到所有需要加载的资产
    TStaticArray<Texture*, 6> textures;
    TArray<Path>              skybox_textures = PathUtils::FilterPath(
        path_,
        [](const Path& p) {
            const String name = p.GetFileName();
            if (name.contains(L"_Left") || name.contains(L"_Right") || name.contains(L"_Top") || name.contains(L"_Bottom") ||
                name.contains(L"_Front") || name.contains(L"_Back"))
            {
                return true;
            }
            return false;
        },
        EPathFilterType::FilterFile,
        false
    );
    if (skybox_textures.size() != 6)
    {
        LOG_WARNING_CATEGORY(TextureCube, L"加载立方体贴图 {} 时找到了数量不符合要求的贴图", path_.ToRelativeString());
    }


    auto LoadCubeFace = [&skybox_textures, this, &textures](const String& subfix, int index) -> bool {
        auto path = ContainerUtils::First(skybox_textures, [&subfix, &textures](const auto& p) { return p.GetFileName().contains(subfix); });
        if (path)
        {
            textures[index] = Texture::Create(*path, ETextureUsage::SkyboxFace, sampler_info_, vk::ImageLayout::eTransferSrcOptimal);
        }
        else
        {
            LOG_ERROR_CATEGORY(TextureCube, L"加载立方体贴图 {} 时未找到 {} 贴图", path_.ToRelativeString(), subfix);
            return false;
        }
        if (width_ == 0 || height_ == 0)
        {
            width_  = textures[index]->GetWidth();
            height_ = textures[index]->GetHeight();
            if (width_ != height_)
            {
                LOG_ERROR_CATEGORY(TextureCube, L"{}: 立方体贴图的六个面的宽高必须相等", path_.ToRelativeString());
                return false;
            }
        }
        else
        {
            if (textures[index]->GetWidth() != width_ || textures[index]->GetHeight() != height_)
            {
                LOG_ERROR_CATEGORY(TextureCube, L"{}: 立方体贴图的六个面的宽高必须一致", path_.ToRelativeString());
                return false;
            }
        }
        return true;
    };
    // +x
    if (!LoadCubeFace(L"_Right", 0))
    {
        if (textures[0])
        {
            delete textures[0];
            textures[0] = nullptr;
        }
        return;
    }
    // -x
    if (!LoadCubeFace(L"_Left", 1))
    {
        if (textures[1])
        {
            delete textures[1];
            textures[1] = nullptr;
        }
        return;
    }
    // +y
    if (!LoadCubeFace(L"_Top", 2))
    {
        if (textures[2])
        {
            delete textures[2];
            textures[2] = nullptr;
        }
        return;
    }
    // -y
    if (!LoadCubeFace(L"_Bottom", 3))
    {
        if (textures[3])
        {
            delete textures[3];
            textures[3] = nullptr;
        }
        return;
    }
    // +z
    if (!LoadCubeFace(L"_Front", 4))
    {
        if (textures[4])
        {
            delete textures[4];
            textures[4] = nullptr;
        }
        return;
    }
    // -z
    if (!LoadCubeFace(L"_Back", 5))
    {
        if (textures[5])
        {
            delete textures[5];
            textures[5] = nullptr;
        }
        return;
    }
    // 复制内存到立方体贴图
    // 创建立方体贴图的Buffer
    ImageInfo image_info    = {};
    image_info.array_layers = 6;
    image_info.format       = vk::Format::eR8G8B8A8Srgb;
    image_info.image_type   = vk::ImageType::e2D;
    image_info.mip_levels   = 1;
    image_info.sample_count = vk::SampleCountFlagBits::e1;
    image_info.tiling       = vk::ImageTiling::eOptimal;
    image_info.usage        = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    image_info.width        = width_;
    image_info.height       = height_;
    image_info.depth        = 1;
    image_info.create_flags = vk::ImageCreateFlagBits::eCubeCompatible;
    image_info.name         = GetPath().ToRelativeAnsiString();

    rhi_texture_ = new RHI::Vulkan::Texture(image_info);
    rhi_texture_->Initialize();

    auto& pool = VulkanContext::Get()->GetCommandPool();
    // 执行图像复制

    for (int i = 0; i < 6; i++)
    {
        pool->TransitionImageLayout(
            GetLowlevelImage(), GetLowlevelFormat(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, 1, i
        );
        vk::ImageCopy copy;
        copy.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        copy.srcSubresource.layerCount     = 1;
        copy.srcOffset                     = {{0, 0, 0}};
        copy.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        copy.dstSubresource.baseArrayLayer = i;
        copy.dstSubresource.layerCount     = 1;
        copy.dstOffset                     = {{0, 0, 0}};
        copy.extent                        = {{(uint32_t)width_, (uint32_t)height_, 1}};
        pool->CopyImage(textures[i]->GetLowlevelImage(), GetLowlevelImage(), {copy});
        pool->TransitionImageLayout(
            GetLowlevelImage(), GetLowlevelFormat(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 1, 1, i
        );
    }
    // TODO: 使用Image创建而不是使用LogicalDevice创建
    vk::ImageViewCreateInfo view_create_info;
    view_create_info.image            = GetLowlevelImage();
    view_create_info.viewType         = vk::ImageViewType::eCube;
    view_create_info.format           = GetLowlevelFormat();
    view_create_info.components       = {vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA};
    view_create_info.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
    view_create_info.subresourceRange.layerCount = 6;
    rhi_texture_view_         = new ImageView(VulkanContext::Get()->GetLogicalDevice()->GetHandle().createImageView(view_create_info));
    rhi_sampler_              = &Sampler::GetDefaultSampler();
    // 创建对应的View
    view_create_info.viewType = vk::ImageViewType::e2D;
    view_create_info.subresourceRange.layerCount = 1;
    for (int i = 0; i < 6; i++)
    {
        view_create_info.subresourceRange.baseArrayLayer = i;
        view_names_[i]                                   = image_info.name + std::to_string(i);
        views_[i] = new ImageView(VulkanContext::Get()->GetLogicalDevice()->GetHandle().createImageView(view_create_info), view_names_[i].c_str());
    }
    // 释放ResourceManager持有的Texture资源, 因为它们现在的Layout是TransferSrc
    for (int i = 0; i < 6; i++)
    {
        ResourceManager::Get()->DestroyResource(textures[i]->GetPath());
    }
}

TextureCube::~TextureCube()
{
    for (int i = 0; i < 6; i++)
    {
        delete views_[i];
    }
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
