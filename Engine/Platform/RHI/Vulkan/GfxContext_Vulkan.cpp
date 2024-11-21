/**
 * @file GfxContext_Vulkan.cpp
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#include "GfxContext_Vulkan.h"
#include "Core/Base/EString.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Platform/PlatformLogcat.h"
#include "Platform/Config/RHIConfig.h"

void PostProcessVulkanExtensions(core::Array<core::String>& extensions) {}

namespace platform::rhi::vulkan
{
GfxContext_Vulkan::GfxContext_Vulkan() = default;

GraphicsAPI GfxContext_Vulkan::GetAPI() const
{
    return GraphicsAPI::Vulkan;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      severity, VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void*                      user_data)
{
    const char* message_type = nullptr;
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
    {
        message_type = "General";
    }
    else if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
    {
        message_type = "Validation";
    }
    else if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
    {
        message_type = "Performance";
    }
    else if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT)
    {
        message_type = "Device Address Binding";
    }
    else
    {
        message_type = "Unknown";
    }
    if (severity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "[{}] {}", message_type, callback_data->pMessage);
    }
    else if (severity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        LOGGER.Warn(logcat::Platform_RHI_Vulkan, "[{}] {}", message_type, callback_data->pMessage);
    }
    else if (severity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "[{}] {}", message_type, callback_data->pMessage);
    }
    else
    {
        LOGGER.Debug(logcat::Platform_RHI_Vulkan, "[{}] {}", message_type, callback_data->pMessage);
    }
    return VK_FALSE;
}

void GfxContext_Vulkan::Initialize()
{
    LOGGER.Info(logcat::Platform_RHI_Vulkan, "Initializing Vulkan Graphics API...");
    auto*             rhi_cfg  = core::GetConfig<RHIConfig>();
    auto*             core_cfg = core::GetConfig<core::CoreConfig>();
    VkApplicationInfo app_info;
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName   = core_cfg->GetAppName().Data();
    app_info.applicationVersion = VK_MAKE_VERSION(
        core_cfg->GetAppVersion().major,
        core_cfg->GetAppVersion().minor,
        core_cfg->GetAppVersion().patch
    );
    app_info.pEngineName   = "Elbow Engine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.apiVersion    = VK_API_VERSION_1_3;

    auto& extensions = rhi_cfg->GetVulkanExtensionNames();
    Event_PostProcessVulkanExtensions.InvokeOnce(extensions);
    for (auto& extension: extensions)
    {
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "Enabled extension: {}", extension);
    }

    VkInstanceCreateInfo instance_info;
    instance_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    LOGGER.Info(logcat::Platform_RHI_Vulkan, "Enable validation layer: {}", rhi_cfg->GetEnableValidationLayer());

    const char* validation_layer_names[1] = {*rhi_cfg->GetValidationLayerName()};

    VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
    debug_info.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_info.messageSeverity                    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_info.pfnUserCallback = DebugCallback;
    if (rhi_cfg->GetEnableValidationLayer())
    {
        instance_info.enabledLayerCount   = 1;
        instance_info.ppEnabledLayerNames = validation_layer_names;
    }


}

void GfxContext_Vulkan::Deinitialize() {}
}
