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

RHI_VULKAN_NAMESPACE_BEGIN

void ValidationLayer::Initialize() {
    if constexpr (!sEnableValidationLayer) {
        LOG_INFO_CATEGORY(Vulkan, L"启用验证层: false");
        return;
    } else {
        LOG_INFO_CATEGORY(Vulkan, L"启用验证层: true");
        if (!mAttachedVulkanInstance) {
            LOG_ERROR_CATEGORY(Vulkan, L"初始化验证层时，Vulkan实例为空");
            return;
        }
        vk::DebugUtilsMessengerCreateInfoEXT CreateInfo{};
        CreateInfo
            .setMessageSeverity(
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            )
            .setMessageType(
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            )
            .setPfnUserCallback(&ThisClass::DebugCallBack)
            .setPUserData(nullptr);
        const auto& Dispatcher = mAttachedVulkanInstance->GetDynamicDispatcher();
        mDebugMessengerCallback =
            mAttachedVulkanInstance->GetVulkanInstance().createDebugUtilsMessengerEXT(
                CreateInfo, nullptr, Dispatcher
            );
        LOG_INFO_CATEGORY(Vulkan, L"验证层初始化完成");
    }
}

void ValidationLayer::Finalize() {
    if (!sEnableValidationLayer) return;
    if (mAttachedVulkanInstance && mAttachedVulkanInstance->IsValid() && mDebugMessengerCallback) {
        const auto& Dispatcher = mAttachedVulkanInstance->GetDynamicDispatcher();
        mAttachedVulkanInstance->GetVulkanInstance().destroyDebugUtilsMessengerEXT(
            mDebugMessengerCallback, nullptr, Dispatcher
        );
        LOG_INFO_CATEGORY(Vulkan, L"清理验证层完成");
    } else {
        LOG_WARNING_CATEGORY(Vulkan, L"销毁验证层时，验证层本身或其AttachedVulkanInstance失效");
    }
}

VkBool32 ValidationLayer::DebugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT      InMessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             InMessageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
) {
    switch (InMessageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_WARNING_CATEGORY(
            Vulkan,
            L"Validation Layer: {} [Message Type: {}]",
            StringUtils::FromAnsiString(pCallbackData->pMessage),
            InMessageType
        );
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOG_ERROR_CATEGORY(
            Vulkan,
            L"Validation Layer: {} [Message Type: {}]",
            StringUtils::FromAnsiString(pCallbackData->pMessage),
            InMessageType
        );
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
    }
    // 当调试验证层本身时返回VK_TRUE
    return VK_FALSE;
}

ValidationLayer& ValidationLayer::SetAttachedVulkanInstance(Instance* InInstance) noexcept {
    mAttachedVulkanInstance = InInstance;
    return *this;
}

RHI_VULKAN_NAMESPACE_END
