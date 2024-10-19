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
#include "Texture.h"


namespace rhi::vulkan
{
struct SamplerInfo;
}
namespace res
{
class Mesh;
class Texture;
}   // namespace Resource
class IResource;

namespace res
{
class ResourceManager : public Singleton<ResourceManager>
{
public:
    ResourceManager() = default;

    ~ResourceManager() override;

    // 注册一个资产（向mReourceMap中添加已经加载完成的资产）
    // 如果InResourcePath已存在则会执行替换
    void RegisterResource(const Path& path, IResource* resource);

    // 获取一个资产
    IResource* GetResource(const Path& path);

    template<typename T>
        requires std::derived_from<T, IResource>
    T* GetResource(const Path& path)
    {
        return dynamic_cast<T*>(GetResource(path));
    }

    void DestroyAllResources();

    void DestroyResource(const Path& path);

private:
    // TODO: Resource与Vulkan GPU Resource的映射
    // TODO: 寻找更好的自动化资源管理方式
    HashMap<Path, IResource*> resource_map_;
};
}
