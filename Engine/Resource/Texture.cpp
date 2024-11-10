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
#include "Logcat.h"
#include "PlatformEvents.h"
#include "ResourceManager.h"
#include "RHI/Vulkan/VulkanContext.h"

#include "FileSystem/Folder.h"
#include "RHI/Vulkan/Render/CommandPool.h"
#include "tinyexr.h"

using namespace rhi::vulkan;

namespace res
{

Texture::Texture(const platform::File& path, const ETextureUsage usage, const SamplerInfo& sampler_info, const vk::ImageLayout t) :
    path_(path), usage_(usage), sampler_info_(sampler_info), init_transition_to_layout_(t)
{
}

Texture* Texture::Create(const platform::File& path, ETextureUsage usage, const SamplerInfo& sampler_info, vk::ImageLayout init_transition)
{
    // 所有的Load操作都发生在没有注册的情况下
    // 因为只能走Create创建 这就保证了已经加载的资源不会走这个函数重新加载
    auto* cached_texture = ResourceManager::Get()->GetResource<Texture>(path);
    if (cached_texture == nullptr)
    {
        // 指针由ResourceManager管理
        cached_texture = New<Texture>(path, usage, sampler_info, init_transition);
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
        LOGGER.Error(logcat::Resource, "Resource {} does not exist", path_.GetRelativePath());
        return;
    }
    const core::String PathStr = path_.GetAbsolutePath();

    data_ = stbi_load(PathStr.Data(), &width_, &height_, &channels_, STBI_rgb_alpha);
    if (!data_)
    {
        LOGGER.Error(logcat::Resource, "Failed to load texture {}", PathStr);
        return;
    }

    // 加载底层RHI资源
    ImageInfo texture_info      = {};
    texture_info.height         = height_;
    texture_info.width          = width_;
    texture_info.image_type     = vk::ImageType::e2D;
    texture_info.usage          = vk::ImageUsageFlagBits::eSampled;
    texture_info.name           = path_.GetRelativePath();
    texture_info.initial_layout = init_transition_to_layout_;
    rhi_texture_                = New<rhi::vulkan::Texture>(texture_info, data_);
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

rhi::vulkan::Texture* Texture::GetRHIResource()
{
    return rhi_texture_;
}

Texture* Texture::GetDefaultLackTexture()
{
    // TODO: ResourceConfig
    return nullptr;
}

vk::Image Texture::GetLowLevelImage() const
{
    return rhi_texture_->GetHandle();
}

vk::Format Texture::GetLowLevelFormat() const
{
    return rhi_texture_->GetFormat();
}

TextureCube* TextureCube::Create(const platform::File& cube_folder, const rhi::vulkan::SamplerInfo& sampler_info)
{
    // 所有的Load操作都发生在没有注册的情况下
    // 因为只能走Create创建 这就保证了已经加载的资源不会走这个函数重新加载
    auto* cached_texture = ResourceManager::Get()->GetResource<TextureCube>(cube_folder);
    if (cached_texture == nullptr)
    {
        // 指针由ResourceManager管理
        cached_texture = New<TextureCube>(cube_folder, sampler_info);
        cached_texture->Load();
        ResourceManager::Get()->RegisterResource(cube_folder, cached_texture);
        cached_texture->ReleaseMemoryCPU();
    }
    return cached_texture;
}

TextureCube::TextureCube(const platform::File& cube_folder, const rhi::vulkan::SamplerInfo& sampler_info)
{
    path_         = cube_folder;
    sampler_info_ = sampler_info;
    usage_        = ETextureUsage::SkyboxCube;
}

void TextureCube::Load()
{
    // 找到所有需要加载的资产
    core::StaticArray<Texture*, 6> textures;
    core::Array<core::String>      skybox_textures = platform::Folder::ListFilesLambda(path_.GetAbsolutePath(), [](core::StringView p) {
        const core::String name = p;
        if (name.Contains("_Left") || name.Contains("_Right") || name.Contains("_Top") || name.Contains("_Bottom") || name.Contains("_Front") ||
            name.Contains("_Back"))
        {
            return true;
        }
        return false;
    });
    if (skybox_textures.size() != 6)
    {
        LOGGER.Warn(logcat::Resource, "Cubemap texture {} count mismatch, requires 6, gets {}", path_.GetRelativePath(), skybox_textures.size());
    }


    auto LoadCubeFace = [&skybox_textures, this, &textures](const core::String& subfix, int index) -> bool {
        if (auto path = ContainerUtils::First(skybox_textures, [&subfix, &textures](const core::String& p) { return p.Contains(subfix); }))
        {
            textures[index] = Texture::Create(*path, ETextureUsage::SkyboxFace, sampler_info_, vk::ImageLayout::eTransferSrcOptimal);
        }
        else
        {
            LOGGER.Error(logcat::Resource, "Cubemap texture {} not found {}", path_.GetRelativePath(), subfix);
            return false;
        }
        if (width_ == 0 || height_ == 0)
        {
            width_  = textures[index]->GetWidth();
            height_ = textures[index]->GetHeight();
            if (width_ != height_)
            {
                LOGGER.Error(
                    logcat::Resource,
                    "Cubemap texture {} width and height must be equal, now width = {}, height = {}",
                    path_.GetRelativePath(),
                    width_,
                    height_
                );
                return false;
            }
        }
        else
        {
            if (textures[index]->GetWidth() != width_ || textures[index]->GetHeight() != height_)
            {
                LOGGER.Error(
                    logcat::Resource,
                    "Cubemap texture {} width and height must be equal, now width = {}, height = {}, required width = {}, height = {}",
                    path_.GetRelativePath(),
                    textures[index]->GetWidth(),
                    textures[index]->GetHeight(),
                    width_,
                    height_
                );
                return false;
            }
        }
        return true;
    };
    // +x
    if (!LoadCubeFace("_Right", 0))
    {
        if (textures[0])
        {
            Delete(textures[0]);
            textures[0] = nullptr;
        }
        return;
    }
    // -x
    if (!LoadCubeFace("_Left", 1))
    {
        if (textures[1])
        {
            Delete(textures[1]);
            textures[1] = nullptr;
        }
        return;
    }
    // +y
    if (!LoadCubeFace("_Top", 2))
    {
        if (textures[2])
        {
            Delete(textures[2]);
            textures[2] = nullptr;
        }
        return;
    }
    // -y
    if (!LoadCubeFace("_Bottom", 3))
    {
        if (textures[3])
        {
            Delete(textures[3]);
            textures[3] = nullptr;
        }
        return;
    }
    // +z
    if (!LoadCubeFace("_Front", 4))
    {
        if (textures[4])
        {
            Delete(textures[4]);
            textures[4] = nullptr;
        }
        return;
    }
    // -z
    if (!LoadCubeFace("_Back", 5))
    {
        if (textures[5])
        {
            Delete(textures[5]);
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
    image_info.name         = path_.GetRelativePath();

    rhi_texture_ = New<rhi::vulkan::Texture>(image_info);
    rhi_texture_->Initialize();

    auto& pool = VulkanContext::Get()->GetCommandPool();
    // 执行图像复制

    for (int i = 0; i < 6; i++)
    {
        pool->TransitionImageLayout(
            GetLowLevelImage(), GetLowLevelFormat(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, 1, i
        );
        vk::ImageCopy copy;
        copy.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        copy.srcSubresource.layerCount     = 1;
        copy.srcOffset                     = {{0, 0, 0}};
        copy.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        copy.dstSubresource.baseArrayLayer = i;
        copy.dstSubresource.layerCount     = 1;
        copy.dstOffset                     = {{0, 0, 0}};
        copy.extent                        = {{static_cast<uint32_t>(width_), static_cast<uint32_t>(height_), 1}};
        pool->CopyImage(textures[i]->GetLowLevelImage(), GetLowLevelImage(), {copy});
        pool->TransitionImageLayout(
            GetLowLevelImage(), GetLowLevelFormat(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 1, 1, i
        );
    }
    // TODO: 使用Image创建而不是使用LogicalDevice创建
    vk::ImageViewCreateInfo view_create_info;
    view_create_info.image            = GetLowLevelImage();
    view_create_info.viewType         = vk::ImageViewType::eCube;
    view_create_info.format           = GetLowLevelFormat();
    view_create_info.components       = {vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA};
    view_create_info.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
    view_create_info.subresourceRange.layerCount = 6;
    rhi_texture_view_         = New<ImageView>(VulkanContext::Get()->GetLogicalDevice()->GetHandle().createImageView(view_create_info));
    rhi_sampler_              = &Sampler::GetDefaultSampler();
    // 创建对应的View
    view_create_info.viewType = vk::ImageViewType::e2D;
    view_create_info.subresourceRange.layerCount = 1;
    for (int i = 0; i < 6; i++)
    {
        view_create_info.subresourceRange.baseArrayLayer = i;
        view_names_[i]                                   = image_info.name + core::String::FromInt(i);
        views_[i] = New<ImageView>(VulkanContext::Get()->GetLogicalDevice()->GetHandle().createImageView(view_create_info), view_names_[i].Data());
    }
    // 释放ResourceManager持有的Texture资源, 因为它们现在的Layout是TransferSrc
    for (int i = 0; i < 6; i++)
    {
        ResourceManager::Get()->DestroyResource(platform::File(textures[i]->GetRelativePath()));
    }
}

TextureCube::~TextureCube()
{
    for (int i = 0; i < 6; i++)
    {
        Delete(views_[i]);
    }
}

}   // namespace res

static void Load_Default_Engine_Texture_Resource(Texture** out_texture, ImageView** out_texture_view)
{
    // TODO: DO load
}

struct RegisterOnEngineDefaultTextureLoad
{
    RegisterOnEngineDefaultTextureLoad() { platform::OnRequestLoadDefaultLackTexture.Bind(Load_Default_Engine_Texture_Resource);
    }
};

static RegisterOnEngineDefaultTextureLoad Register_On_Engine_Default_Texture_Load;
