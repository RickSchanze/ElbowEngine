/**
 * @file ValidationLayer.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once
#include "Interface/IRHIResource.h"
#include "VulkanCommon.h"

#include <vulkan/vulkan.hpp>

RHI_VULKAN_NAMESPACE_BEGIN
class Instance;

class ValidationLayer final : public IRHIResource {
public:
    typedef ValidationLayer ThisClass;

    explicit ValidationLayer(Instance* InParentInstance) noexcept : mAttachedVulkanInstance(InParentInstance) {}

    void Initialize();
    void Finialize();

    bool IsValid() const { return static_cast<bool>(mDebugMessengerCallback); }

#if ELBOW_DEBUG
    static constexpr bool sEnableValidationLayer = true;
#else
    static constexpr bool sEnableValidationLayer = false;
#endif
    static inline Array<const char*> gValidationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    static constexpr bool               IsEnable() noexcept { return sEnableValidationLayer; }
    static constexpr Array<const char*> GetValidationLayerNames() noexcept { return gValidationLayers; }

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
        VkDebugUtilsMessageSeverityFlagBitsEXT InMessageSeverity, VkDebugUtilsMessageTypeFlagsEXT InMessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
    );

    [[nodiscard]] vk::DebugUtilsMessengerEXT GetDebugMessenger() const { return mDebugMessengerCallback; }
    ValidationLayer&                         SetAttachedVulkanInstance(Instance* InInstance) noexcept;


private:
    vk::DebugUtilsMessengerEXT mDebugMessengerCallback;

    // 验证层附加的Instance对象
    Instance* mAttachedVulkanInstance = nullptr;
};

RHI_VULKAN_NAMESPACE_END
