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

ResourceManager::~ResourceManager() {
    for (const auto& Resource: mResourceMap | std::views::values) {
        delete Resource;
    }
}

void ResourceManager::RegisterResource(const Path& InResourcePath, IResource* InResource) {
    if (InResource == nullptr || !InResource->IsValid()) {
        LOG_WARNING_CATEGORY(Resource, L"注册资产失败, {}对应的资产无效.", InResourcePath.ToString());
        return;
    }
    if (mResourceMap.contains(InResourcePath)) {
        LOG_WARNING_CATEGORY(Resource, L"{}对应资产已存在,执行替换.", InResourcePath.ToString());
    }
    mResourceMap[InResourcePath] = InResource;
}

IResource* ResourceManager::GetResource(const Path& InResourcePath) {
    return mResourceMap.contains(InResourcePath) ? mResourceMap[InResourcePath] : nullptr;
}
