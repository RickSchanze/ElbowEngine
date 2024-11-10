/**
 * @file ResourceManager.h
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Singleton/Singleton.h"
#include "Texture.h"
#include "FileSystem/File.h"


namespace platform
{
class File;
}
namespace rhi::vulkan
{
struct SamplerInfo;
}
namespace res
{
class Mesh;
class Texture;
}   // namespace res
class IResource;

namespace res
{
class ResourceManager : public Singleton<ResourceManager>
{
public:
    ResourceManager() = default;

    ~ResourceManager() override;

    // 注册一个资产（向mResourceMap中添加已经加载完成的资产）
    // 如果InResourcePath已存在则会执行替换
    void RegisterResource(const platform::File& path, IResource* resource);

    // 获取一个资产
    IResource* GetResource(const platform::File& path);

    template <typename T>
        requires std::derived_from<T, IResource>
    T* GetResource(const platform::File& path)
    {
        return dynamic_cast<T*>(GetResource(path));
    }

    void DestroyAllResources();

    void DestroyResource(const platform::File& path);

private:
    // TODO: 寻找更好的自动化资源管理方式
    core::HashMap<platform::File, IResource*> resource_map_ = {};
};
}
