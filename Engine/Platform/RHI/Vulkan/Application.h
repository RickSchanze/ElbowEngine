/**
 * @file Application.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "Core/CoreDef.h"
#include "Instance.h"
#include "VulkanContext.h"

namespace rhi::vulkan
{
class VulkanApplication
{
public:
    typedef VulkanApplication ThisClass;

    ~VulkanApplication();

    VulkanApplication& SetAppName(const core::String& app_name) noexcept;
    VulkanApplication& SetEngineName(const core::String& engine_name) noexcept;
    VulkanApplication& SetAppVersion(uint32_t app_version) noexcept;
    VulkanApplication& SetEngineVersion(uint32_t engine_version) noexcept;
    VulkanApplication& SetApiVersion(uint32_t api_version) noexcept;
    VulkanApplication& SetExtensions(const core::Array<const char*>& extensions) noexcept;
    VulkanApplication& SetWindowSurface(core::UniquePtr<SurfaceBase> surface) noexcept;

    void Initialize();
    void Finalize();

    // 删除所有默认加载的资产
    void DestroyDefaultLoadedResource();

    const core::String&       GetAppName() const noexcept { return app_name_; }
    const core::String&       GetEngineName() const noexcept { return engine_name_; }
    uint32_t                  GetAppVersion() const noexcept { return app_version_; }
    uint32_t                  GetEngineVersion() const noexcept { return engine_version_; }
    uint32_t                  GetApiVersion() const noexcept { return api_version_; }
    core::SharedPtr<Instance> GetVulkanInstance() noexcept { return vulkan_instance_; }
    bool                      IsValid() const noexcept { return vulkan_instance_->IsValid(); }
    VulkanContext&            GetContext() const noexcept { return *vulkan_context_; }

protected:
    // 初始化Instance
    void CreateInstance();

private:
    core::String app_name_       = "Elbow Engine Editor";
    core::String engine_name_    = "Elbow Engine";
    uint32_t     app_version_    = VK_MAKE_VERSION(1, 0, 0);
    uint32_t     engine_version_ = VK_MAKE_VERSION(1, 0, 0);
    uint32_t     api_version_    = VK_API_VERSION_1_3;

    core::SharedPtr<Instance>      vulkan_instance_;
    core::UniquePtr<SurfaceBase>   surface_;
    core::UniquePtr<VulkanContext> vulkan_context_;

    core::Array<const char*> extensions_;
};
}
