/**
 * @file ResourceManager.cpp
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#include "ResourceManager.h"

#include "CoreGlobal.h"
#include "Interface/IResource.h"

#include <ranges>

RESOURCE_NAMESPACE_BEGIN

ResourceManager::~ResourceManager()
{
    for (const auto& Resource: resource_map_ | std::views::values)
    {
        delete Resource;
    }
}

void ResourceManager::RegisterResource(const Path& InResourcePath, IResource* InResource)
{
    if (InResource == nullptr || !InResource->IsValid())
    {
        LOG_WARNING_CATEGORY(Resource, L"注册资产失败, {}对应的资产无效.", InResourcePath.ToString());
        return;
    }
    if (resource_map_.contains(InResourcePath))
    {
        LOG_WARNING_CATEGORY(Resource, L"{}对应资产已存在,执行替换.", InResourcePath.ToString());
    }
    resource_map_[InResourcePath] = InResource;
}

IResource* ResourceManager::GetResource(const Path& InResourcePath)
{
    return resource_map_.contains(InResourcePath) ? resource_map_[InResourcePath] : nullptr;
}

Texture* ResourceManager::GetOrCreateTexture(const Path& path, ETextureUsage usage, const RHI::Vulkan::SamplerInfo& sampler_info)
{
    auto* texture = GetResource<Texture>(path);
    if (texture == nullptr)
    {
        Texture* new_texture = Texture::Create(path, usage, sampler_info);
        return new_texture;
    }

    return texture;
}

RESOURCE_NAMESPACE_END
