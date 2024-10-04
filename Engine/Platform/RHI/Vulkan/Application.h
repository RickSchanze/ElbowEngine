/**
 * @file Application.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Instance.h"
#include "VulkanCommon.h"
#include "VulkanContext.h"

namespace rhi::vulkan
{
class VulkanApplication
{
public:
    typedef VulkanApplication ThisClass;

    ~VulkanApplication();

    VulkanApplication& SetAppName(const String& app_name) noexcept;
    VulkanApplication& SetEngineName(const String& engine_name) noexcept;
    VulkanApplication& SetAppVersion(uint32_t app_version) noexcept;
    VulkanApplication& SetEngineVersion(uint32_t engine_version) noexcept;
    VulkanApplication& SetApiVersion(uint32_t api_version) noexcept;
    VulkanApplication& SetExtensions(const TArray<const char*>& extensions) noexcept;
    VulkanApplication& SetWindowSurface(TUniquePtr<SurfaceBase> surface) noexcept;

    void Initialize();
    void Finalize();

    // 删除所有默认加载的资产
    void DestroyDefaultLoadedResource();

    const String&        GetAppName() const noexcept { return app_name_; }
    const String&        GetEngineName() const noexcept { return engine_name_; }
    uint32_t             GetAppVersion() const noexcept { return app_version_; }
    uint32_t             GetEngineVersion() const noexcept { return engine_version_; }
    uint32_t             GetApiVersion() const noexcept { return api_version_; }
    TSharedPtr<Instance> GetVulkanInstance() noexcept { return vulkan_instance_; }
    bool                 IsValid() const noexcept { return vulkan_instance_->IsValid(); }
    VulkanContext&       GetContext() const noexcept { return *vulkan_context_; }

protected:
    // 初始化Instance
    void CreateInstance();

private:
    String   app_name_       = L"Elbow Engine Editor";
    String   engine_name_    = L"Elbow Engine";
    uint32_t app_version_    = VK_MAKE_VERSION(1, 0, 0);
    uint32_t engine_version_ = VK_MAKE_VERSION(1, 0, 0);
    uint32_t api_version_    = VK_API_VERSION_1_3;

    TSharedPtr<Instance>      vulkan_instance_;
    TUniquePtr<SurfaceBase>   surface_;
    TUniquePtr<VulkanContext> vulkan_context_;

    TArray<const char*> extensions_;
};
}
