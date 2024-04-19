/**
 * @file ValidationLayer.cpp
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#include "ValidationLayer.h"

#include "CoreGlobal.h"

RHI_VULKAN_NAMESPACE_BEGIN

void ValidationLayer::Initialize() {
    if constexpr (!sEnableValidationLayer) {
        LOG_INFO_CATEGORY(Vulkan, L"启用验证层: false");
        return;
    } else {
        LOG_INFO_CATEGORY(Vulkan, L"启用验证层: true");
        if (!mVulkanInstance) {
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
            );

        LOG_INFO_CATEGORY(Vulkan, "验证层初始化完成");
    }
}

void ValidationLayer::Finalize() {
    if (!sEnableValidationLayer) return;
    if (mVulkanInstance) {
        const auto& Dispatcher = mVulkanInstance.GetDynamicDispatcher();
        mVulkanInstance.GetVulkanInstance().destroyDebugUtilsMessengerEXT(
            mDebugMessengerCallback, nullptr, Dispatcher
        );
    } else {
    }
}

RHI_VULKAN_NAMESPACE_END
