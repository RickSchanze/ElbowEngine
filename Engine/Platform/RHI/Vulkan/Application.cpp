/**
 * @file Application.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "Application.h"

#include "CoreGlobal.h"
#include "Utils/StringUtils.h"
#include "ValidationLayer.h"
RHI_VULKAN_NAMESPACE_BEGIN

VulkanApplication::~VulkanApplication() {
    if (mVulkanInstance.IsValid()) {
        Finalize();
    }
}

VulkanApplication& VulkanApplication::SetAppName(const String& InAppName) noexcept {
    mAppName = InAppName;
    return *this;
}

VulkanApplication& VulkanApplication::SetEngineName(const String& InEngineName) noexcept {
    mEngineName = InEngineName;
    return *this;
}

VulkanApplication& VulkanApplication::SetAppVersion(const uint32_t InAppVersion) noexcept {
    mAppVersion = InAppVersion;
    return *this;
}

VulkanApplication& VulkanApplication::SetEngineVersion(const uint32_t InEngineVersion) noexcept {
    mEngineVersion = InEngineVersion;
    return *this;
}

VulkanApplication& VulkanApplication::SetApiVersion(const uint32_t InApiVersion) noexcept {
    mApiVersion = InApiVersion;
    return *this;
}

VulkanApplication& VulkanApplication::SetExtensions(const Array<const char*>& InExtensions
) noexcept {
    Extensions = InExtensions;
    return *this;
}

void VulkanApplication::Initialize() {
    CreateInstance();
    mVulkanInstance.Initialize();
}

void VulkanApplication::Finalize() {
    mVulkanInstance.Finalize();
}

void VulkanApplication::CreateInstance() {
    vk::ApplicationInfo AppInfo;
    const AnsiString    AppName    = StringUtils::ToAnsiString(mAppName);
    const AnsiString    EngineName = StringUtils::ToAnsiString(mEngineName);
    AppInfo.setPApplicationName(AppName.c_str())
        .setPEngineName(EngineName.c_str())
        .setApiVersion(mApiVersion)
        .setEngineVersion(mEngineVersion)
        .setApplicationVersion(mAppVersion);
    vk::InstanceCreateInfo InstanceCreateInfo;
    if (Extensions.empty()) {
        LOG_WARNING_CATEGORY(Vulkan, L"扩展数组列表为空");
    }
    if (ValidationLayer::sEnableValidationLayer) {
        Extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        InstanceCreateInfo.setEnabledLayerCount(ValidationLayer::gValidationLayers.size())
            .setPpEnabledLayerNames(ValidationLayer::gValidationLayers.data());
    }
    InstanceCreateInfo.setPApplicationInfo(&AppInfo)
        .setEnabledExtensionCount(Extensions.size())
        .setPpEnabledExtensionNames(Extensions.data());
    mVulkanInstance = Instance(InstanceCreateInfo);
}

RHI_VULKAN_NAMESPACE_END