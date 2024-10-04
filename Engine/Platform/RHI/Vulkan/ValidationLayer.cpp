/**
 * @file ValidationLayer.cpp
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#include "ValidationLayer.h"

#include "CoreGlobal.h"
#include "Instance.h"
#include "Utils/StringUtils.h"

namespace rhi::vulkan
{
void ValidationLayer::Initialize() {
    if constexpr (!sEnableValidationLayer) {
        LOG_INFO_CATEGORY(Vulkan, L"启用验证层: false");
        return;
    } else {
        LOG_INFO_CATEGORY(Vulkan, L"启用验证层: true");
        if (!vulkan_instance_) {
            LOG_ERROR_CATEGORY(Vulkan, L"初始化验证层时，Vulkan实例为空");
            return;
        }
        vk::DebugUtilsMessengerCreateInfoEXT CreateInfo{};
        CreateInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
            .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
            .setPfnUserCallback(&ThisClass::DebugCallBack)
            .setPUserData(nullptr);
        const auto& Dispatcher  = vulkan_instance_->GetDynamicDispatcher();
        debug_messenger_callback_ = vulkan_instance_->GetHandle().createDebugUtilsMessengerEXT(CreateInfo, nullptr, Dispatcher);
        LOG_INFO_CATEGORY(Vulkan, L"验证层初始化完成");
    }
}

void ValidationLayer::DeInitialize() {
    if (!sEnableValidationLayer) return;
    if (vulkan_instance_ && vulkan_instance_->IsValid() && debug_messenger_callback_) {
        const auto& Dispatcher = vulkan_instance_->GetDynamicDispatcher();
        vulkan_instance_->GetHandle().destroyDebugUtilsMessengerEXT(debug_messenger_callback_, nullptr, Dispatcher);
        LOG_INFO_CATEGORY(Vulkan, L"验证层清理完成");
    } else {
        LOG_WARNING_CATEGORY(Vulkan, L"销毁验证层时，验证层本身或其AttachedVulkanInstance失效");
    }
}

void ValidationLayer::Destroy() {
    DeInitialize();
}

VkBool32 ValidationLayer::DebugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT InMessageSeverity, VkDebugUtilsMessageTypeFlagsEXT InMessageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
) {
    switch (InMessageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_WARNING_CATEGORY(
            Vulkan, L"Validation Layer: {} [Message Type: {}]", StringUtils::FromAnsiString(pCallbackData->pMessage), InMessageType
        );
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOG_ERROR_CATEGORY(
            Vulkan, L"Validation Layer: {} [Message Type: {}]", StringUtils::FromAnsiString(pCallbackData->pMessage), InMessageType
        );
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
    }
    // 当调试验证层本身时返回VK_TRUE
    return VK_FALSE;
}

ValidationLayer& ValidationLayer::SetAttachedVulkanInstance(Instance* InInstance) noexcept {
    vulkan_instance_ = InInstance;
    return *this;
}
}
