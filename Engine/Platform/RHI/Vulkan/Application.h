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
    VulkanApplication& SetExtensions(const Array<const char*>& InExtensions) noexcept;

    [[nodiscard]] const String& GetAppName() const noexcept { return mAppName; }
    [[nodiscard]] const String& GetEngineName() const noexcept { return mEngineName; }
    [[nodiscard]] uint32_t      GetAppVersion() const noexcept { return mAppVersion; }
    [[nodiscard]] uint32_t      GetEngineVersion() const noexcept { return mEngineVersion; }
    [[nodiscard]] uint32_t      GetApiVersion() const noexcept { return mApiVersion; }

    void Initialize();
    void Finalize();

    [[nodiscard]] Instance& GetVulkanInstance() noexcept { return mVulkanInstance; }

protected:
    // 初始化Instance
    void CreateInstance();

private:
    String   mAppName = L"Elbow Engine";
    String   mEngineName = L"No Engine";
    uint32_t mAppVersion    = VK_MAKE_VERSION(1, 0, 0);
    uint32_t mEngineVersion = VK_MAKE_VERSION(1, 0, 0);
    uint32_t mApiVersion    = VK_MAKE_API_VERSION(1, 0, 0, 0);

    Instance           mVulkanInstance;
    Array<const char*> Extensions;
};

RHI_VULKAN_NAMESPACE_END