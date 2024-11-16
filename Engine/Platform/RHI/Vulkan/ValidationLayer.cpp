/**
 * @file ValidationLayer.cpp
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#include "ValidationLayer.h"
#include "Core/CoreGlobal.h"
#include "Instance.h"
#include "Platform/PlatformLogcat.h"

namespace rhi::vulkan
{
void ValidationLayer::Initialize()
{
    if constexpr (!sEnableValidationLayer)
    {
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "Validation layer disabled.");
        return;
    }
    else
    {
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "Validation layer enabled.");
        if (!vulkan_instance_)
        {
            LOGGER.Error(logcat::Platform_RHI_Vulkan, "Initialize validation layer failed because vulkan instance is null");
            return;
        }
        vk::DebugUtilsMessengerCreateInfoEXT CreateInfo{};
        CreateInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
            .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
            .setPfnUserCallback(&ThisClass::DebugCallBack)
            .setPUserData(nullptr);
        const auto& Dispatcher    = vulkan_instance_->GetDynamicDispatcher();
        debug_messenger_callback_ = vulkan_instance_->GetHandle().createDebugUtilsMessengerEXT(CreateInfo, nullptr, Dispatcher);
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "Validation layer initialized.");
    }
}

void ValidationLayer::DeInitialize() const
{
    if (!sEnableValidationLayer) return;
    if (vulkan_instance_ && vulkan_instance_->IsValid() && debug_messenger_callback_)
    {
        const auto& Dispatcher = vulkan_instance_->GetDynamicDispatcher();
        vulkan_instance_->GetHandle().destroyDebugUtilsMessengerEXT(debug_messenger_callback_, nullptr, Dispatcher);
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "Validation layer destroyed.");
    }
    else
    {
        LOGGER.Warn(logcat::Platform_RHI_Vulkan, "when destroy validation layer, validation layer itself or its attached vulkan instance is invalid");
    }
}

void ValidationLayer::Destroy()
{
    DeInitialize();
}

VkBool32 ValidationLayer::DebugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data
)
{
    switch (severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOGGER.Warn(logcat::Platform_RHI_Vulkan, "Validation Layer: {} [Message Type: {}]", callback_data->pMessage, type);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOGGER.Error(logcat::Platform_RHI_Vulkan, "Validation Layer: {} [Message Type: {}]", callback_data->pMessage, type);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
    }
    // 当调试验证层本身时返回VK_TRUE
    return VK_FALSE;
}

ValidationLayer& ValidationLayer::SetAttachedVulkanInstance(Instance* InInstance) noexcept
{
    vulkan_instance_ = InInstance;
    return *this;
}
}
