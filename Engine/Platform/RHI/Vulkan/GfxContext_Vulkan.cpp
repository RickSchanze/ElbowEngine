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
#include "Core/Profiler/ProfileMacro.h"
#include "range/v3/all.hpp"

core::Array<VkLayerProperties> available_layers;

static core::Array<VkLayerProperties> GetAvailableLayers()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    core::Array<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
    return available_layers;
}

static bool SupportValidationLayer(core::StringView layer_name)
{
    for (auto& layer: available_layers)
    {
        if (layer_name == layer.layerName)
        {
            return true;
        }
    }
    return false;
}

namespace platform::rhi::vulkan
{
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

static void CreateInstance(core::Ref<VkInstance> instance)
{
    PROFILE_SCOPE_AUTO;
    auto*             rhi_cfg  = core::GetConfig<RHIConfig>();
    auto*             core_cfg = core::GetConfig<core::CoreConfig>();
    VkApplicationInfo app_info{};
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

    core::Array<const char*> extensions;
    if (rhi_cfg->GetEnableValidationLayer())
    {
        extensions.emplace_back("VK_EXT_debug_utils");
    }
    Event_PostProcessVulkanExtensions.InvokeOnce(extensions);
    for (auto& extension: extensions)
    {
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "Enabled extension: {}", extension);
    }

    VkInstanceCreateInfo instance_info{};
    instance_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo        = &app_info;
    instance_info.enabledExtensionCount   = extensions.size();
    instance_info.ppEnabledExtensionNames = extensions.data();

    LOGGER.Info(logcat::Platform_RHI_Vulkan, "Enable validation layer: {}", rhi_cfg->GetEnableValidationLayer());
    VkDebugUtilsMessengerCreateInfoEXT debug_info = {};
    if (rhi_cfg->GetEnableValidationLayer())
    {
        Assert(logcat::Platform_RHI_Vulkan, SupportValidationLayer(rhi_cfg->GetValidationLayerName()), "Validation layer is not supported!");
        const char* validation_layer_names[1] = {*rhi_cfg->GetValidationLayerName()};
        debug_info.sType                      = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_info.messageSeverity            = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_info.pfnUserCallback        = DebugCallback;
        instance_info.enabledLayerCount   = 1;
        instance_info.ppEnabledLayerNames = validation_layer_names;
        instance_info.pNext               = &debug_info;
    }
    else
    {
        instance_info.enabledLayerCount = 0;
        instance_info.pNext             = nullptr;
    }
    VERIFY_VULKAN_RESULT(vkCreateInstance(&instance_info, nullptr, instance.GetPtr()))
}

static void DestroyInstance(core::Ref<VkInstance> instance)
{
    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;
}

GfxContext_Vulkan::GfxContext_Vulkan()
{
    available_layers = GetAvailableLayers();
    LOGGER.Info(logcat::Platform_RHI_Vulkan, "Initializing Vulkan Graphics API...");
    CreateInstance(instance_);
}

GfxContext_Vulkan::~GfxContext_Vulkan()
{
    DestroyInstance(instance_);
}
}
