/**
 * @file Application.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "Application.h"

#include "CoreGlobal.h"
#include "PlatformLogcat.h"

#include "ValidationLayer.h"

namespace rhi::vulkan
{
VulkanApplication::~VulkanApplication()
{
    if (vulkan_instance_->IsValid())
    {
        Finalize();
    }
}

VulkanApplication& VulkanApplication::SetAppName(const core::String& app_name) noexcept
{
    app_name_ = app_name;
    return *this;
}

VulkanApplication& VulkanApplication::SetEngineName(const core::String& engine_name) noexcept
{
    engine_name_ = engine_name;
    return *this;
}

VulkanApplication& VulkanApplication::SetAppVersion(const uint32_t app_version) noexcept
{
    app_version_ = app_version;
    return *this;
}

VulkanApplication& VulkanApplication::SetEngineVersion(const uint32_t engine_version) noexcept
{
    engine_version_ = engine_version;
    return *this;
}

VulkanApplication& VulkanApplication::SetApiVersion(const uint32_t api_version) noexcept
{
    api_version_ = api_version;
    return *this;
}

VulkanApplication& VulkanApplication::SetExtensions(const core::Array<const char*>& extensions) noexcept
{
    extensions_ = extensions;
    return *this;
}

VulkanApplication& VulkanApplication::SetWindowSurface(core::UniquePtr<SurfaceBase> surface) noexcept
{
    surface_ = Move(surface);
    return *this;
}

void VulkanApplication::Initialize()
{
    CreateInstance();
    vulkan_instance_->Initialize();
    vulkan_context_ = VulkanContext::CreateUnique(vulkan_instance_);
}

void VulkanApplication::Finalize()
{
    DestroyDefaultLoadedResource();
    if (vulkan_context_) vulkan_context_->Finalize();
    if (vulkan_instance_) vulkan_instance_->DeInitialize();
}

void VulkanApplication::DestroyDefaultLoadedResource() {}

void VulkanApplication::CreateInstance()
{
    vk::ApplicationInfo AppInfo;
    const core::String  AppName    = app_name_;
    const core::String  EngineName = engine_name_;
    AppInfo.setPApplicationName(AppName.Data())
        .setPEngineName(EngineName.Data())
        .setApiVersion(api_version_)
        .setEngineVersion(engine_version_)
        .setApplicationVersion(app_version_);
    vk::InstanceCreateInfo InstanceCreateInfo;
    if (extensions_.empty())
    {
        LOGGER.Warn(logcat::Platform_RHI_Vulkan, "extensions_ is empty.");
    }
    if (ValidationLayer::sEnableValidationLayer)
    {
        extensions_.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        InstanceCreateInfo.setEnabledLayerCount(ValidationLayer::gValidationLayers.size())
            .setPpEnabledLayerNames(ValidationLayer::gValidationLayers.data());
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "Enable validation layer: ");
        for (const auto& extension: ValidationLayer::gValidationLayers)
        {
            LOGGER.Info(logcat::Platform_RHI_Vulkan, "  {}", extension);
        }
    }
    InstanceCreateInfo.setPApplicationInfo(&AppInfo).setEnabledExtensionCount(extensions_.size()).setPpEnabledExtensionNames(extensions_.data());
    vulkan_instance_ = core::MakeShared<Instance>();
    vulkan_instance_->SetSurface(Move(surface_));
    vulkan_instance_->SetInstanceCreateInfo(InstanceCreateInfo);
}
}   // namespace rhi::vulkan
