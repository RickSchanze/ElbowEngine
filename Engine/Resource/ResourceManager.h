/**
 * @file ResourceManager.h
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Path/Path.h"
#include "Singleton/Singleton.h"


class IResource;
class ResourceManager : public Singleton<ResourceManager> {
public:
    ResourceManager() = default;
    ~ResourceManager();

    // 注册一个资产（向mReourceMap中添加已经加载完成的资产）
    // 如果InResourcePath已存在则会执行替换
    void RegisterResource(const Path& InResourcePath, IResource* InResource);

    // 获取一个资产
    IResource* GetResource(const Path& InResourcePath);

    template<typename T>
        requires std::derived_from<T, IResource>
    T* GetResource(const Path& InResourcePath) {
        return dynamic_cast<T*>(GetResource(InResourcePath));
    }

private:
    // TODO: Resource与Vulkan GPU Resource的映射
    // TODO: 寻找更好的自动化资源管理方式
    THashMap<Path, IResource*> mResourceMap;
};
