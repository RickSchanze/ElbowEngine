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

RHI_VULKAN_NAMESPACE_BEGIN

class VulkanApplication {
public:
    typedef VulkanApplication ThisClass;

    ~VulkanApplication();

    VulkanApplication& SetAppName(const String& InAppName) noexcept;
    VulkanApplication& SetEngineName(const String& InEngineName) noexcept;
    VulkanApplication& SetAppVersion(uint32_t InAppVersion) noexcept;
    VulkanApplication& SetEngineVersion(uint32_t InEngineVersion) noexcept;
    VulkanApplication& SetApiVersion(uint32_t InApiVersion) noexcept;
    VulkanApplication& SetExtensions(const TArray<const char*>& InExtensions) noexcept;
    VulkanApplication& SetWindowSurface(TUniquePtr<SurfaceBase> InSurface) noexcept;

    void Initialize();
    void Finalize();

    void Tick();

    const String&       GetAppName() const noexcept { return mAppName; }
    const String&       GetEngineName() const noexcept { return mEngineName; }
    uint32_t            GetAppVersion() const noexcept { return mAppVersion; }
    uint32_t            GetEngineVersion() const noexcept { return mEngineVersion; }
    uint32_t            GetApiVersion() const noexcept { return mApiVersion; }
    TSharedPtr<Instance> GetVulkanInstance() noexcept { return mVulkanInstance; }
    bool                IsValid() const noexcept { return mVulkanInstance->IsValid(); }
    VulkanContext&      GetContext() const noexcept { return *mContext; }

protected:
    // 初始化Instance
    void CreateInstance();

private:
    String   mAppName       = L"Elbow Engine Editor";
    String   mEngineName    = L"Elbow Engine";
    uint32_t mAppVersion    = VK_MAKE_VERSION(1, 0, 0);
    uint32_t mEngineVersion = VK_MAKE_VERSION(1, 0, 0);
    uint32_t mApiVersion    = VK_API_VERSION_1_3;

    TSharedPtr<Instance>      mVulkanInstance;
    TUniquePtr<SurfaceBase>   mSurface;
    TSharedPtr<VulkanContext> mContext;

    TArray<const char*> Extensions;
};

RHI_VULKAN_NAMESPACE_END
