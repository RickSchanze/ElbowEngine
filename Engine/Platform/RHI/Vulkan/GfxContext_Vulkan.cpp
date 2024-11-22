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
#include "Platform/Config/PlatformConfig.h"
#include "Core/Profiler/ProfileMacro.h"
#include "range/v3/all.hpp"

static core::Array<VkLayerProperties>     available_layers;
static core::Array<VkPhysicalDevice>      physical_devices;
static core::Array<const char*>           required_extensions;

static core::Array<VkPhysicalDevice> GetAvailablePhysicalDevices(VkInstance instance)
{
    uint32_t device_cnt = 0;
    vkEnumeratePhysicalDevices(instance, &device_cnt, nullptr);
    Assert(logcat::Platform_RHI_Vulkan, device_cnt > 0, "No physical device found");
    core::Array<VkPhysicalDevice> physical_devices(device_cnt);
    vkEnumeratePhysicalDevices(instance, &device_cnt, physical_devices.data());
    return physical_devices;
}

static core::Array<VkLayerProperties> GetAvailableLayers()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    core::Array<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
    return available_layers;
}

static bool IsPhysicalDeviceSuitable(VkPhysicalDevice device)
{
    // 看扩展是不是完全支持

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

core::Array<VkExtensionProperties> GfxContext_Vulkan::GetAvailableExtensions(VkPhysicalDevice)
{
    uint32_t extension_count;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    core::Array<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data());
    return available_extensions;
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
    auto*             rhi_cfg  = core::GetConfig<PlatformConfig>();
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

    if (rhi_cfg->GetEnableValidationLayer())
    {
        required_extensions.emplace_back("VK_EXT_debug_utils");
    }
    Event_PostProcessVulkanExtensions.InvokeOnce(required_extensions);
    for (auto& extension: required_extensions)
    {
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "Enabled extension: {}", extension);
    }

    VkInstanceCreateInfo instance_info{};
    instance_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo        = &app_info;
    instance_info.enabledExtensionCount   = required_extensions.size();
    instance_info.ppEnabledExtensionNames = required_extensions.data();

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

static void SelectPhysicalDevice(core::Ref<VkPhysicalDevice> physical_device)
{
    PROFILE_SCOPE_AUTO;
    for (auto candidate: physical_devices) {}
}

GfxContext_Vulkan::GfxContext_Vulkan()
{
    available_layers = GetAvailableLayers();
    LOGGER.Info(logcat::Platform_RHI_Vulkan, "Initializing Vulkan Graphics API...");
    CreateInstance(instance_);
    physical_devices     = GetAvailablePhysicalDevices(instance_);
    SelectPhysicalDevice(physical_device_);
}

GfxContext_Vulkan::~GfxContext_Vulkan()
{
    DestroyInstance(instance_);
}
}

core::StringView VulkanErrorToString(VkResult result)
{
    switch (result)
    {
    case VK_SUCCESS: return "VK_SUCCESS";
    case VK_NOT_READY: return "VK_NOT_READY";
    case VK_TIMEOUT: return "VK_TIMEOUT";
    case VK_EVENT_SET: return "VK_EVENT_SET";
    case VK_EVENT_RESET: return "VK_EVENT_RESET";
    case VK_INCOMPLETE: return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
    default: return "Unknown error";
    }
}
