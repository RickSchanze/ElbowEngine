/**
 * @file ResourceManager.cpp
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#include "ResourceManager.h"

#include "Core/CoreGlobal.h"
#include "Platform/FileSystem/File.h"
#include "Interface/IResource.h"
#include "Logcat.h"
#include "Mesh.h"

#include <ranges>

namespace resource
{
ResourceManager::~ResourceManager() = default;

void ResourceManager::RegisterResource(const platform::File& InResourcePath, IResource* InResource)
{
    if (InResource == nullptr || !InResource->IsValid())
    {
        LOGGER.Error(logcat::Resource, "Fail to register resource, {} is invalid.", InResourcePath.GetRelativePath());
        return;
    }
    if (resource_map_.contains(InResourcePath))
    {
        LOGGER.Warn(logcat::Resource, "Duplicate resource registration: {}", InResourcePath.GetRelativePath());
    }
    resource_map_[InResourcePath] = InResource;
}

IResource* ResourceManager::GetResource(const platform::File& InResourcePath)
{
    return resource_map_.contains(InResourcePath) ? resource_map_[InResourcePath] : nullptr;
}

void ResourceManager::DestroyAllResources()
{
    for (const auto& resource: resource_map_ | std::views::values)
    {
        Delete(resource);
    }
    resource_map_.clear();
}

void ResourceManager::DestroyResource(const platform::File& path)
{
    if (!resource_map_.contains(path))
    {
        LOGGER.Error(logcat::Resource, "Try to destroy non-exist resource: {}", path.GetRelativePath());
        return;
    }
    Delete(resource_map_[path]);
    resource_map_.erase(path);
}
}
