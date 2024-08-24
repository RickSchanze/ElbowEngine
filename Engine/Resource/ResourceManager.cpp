/**
 * @file ResourceManager.cpp
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#include "ResourceManager.h"

#include "CoreGlobal.h"
#include "Interface/IResource.h"
#include "Mesh.h"

#include <ranges>

RESOURCE_NAMESPACE_BEGIN

ResourceManager::~ResourceManager() = default;

void ResourceManager::RegisterResource(const Path& InResourcePath, IResource* InResource)
{
    if (InResource == nullptr || !InResource->IsValid())
    {
        LOG_ERROR_CATEGORY(Resource, L"注册资产失败, {}对应的资产无效.", InResourcePath.ToRelativeString());
        return;
    }
    if (resource_map_.contains(InResourcePath))
    {
        LOG_ERROR_CATEGORY(Resource, L"{}对应资产已存在,执行替换.", InResourcePath.ToRelativeString());
    }
    resource_map_[InResourcePath] = InResource;
}

IResource* ResourceManager::GetResource(const Path& InResourcePath)
{
    return resource_map_.contains(InResourcePath) ? resource_map_[InResourcePath] : nullptr;
}

void ResourceManager::DestroyAllResources()
{
    for (const auto& resource: resource_map_ | std::views::values)
    {
        delete resource;
    }
    resource_map_.clear();
}

RESOURCE_NAMESPACE_END
