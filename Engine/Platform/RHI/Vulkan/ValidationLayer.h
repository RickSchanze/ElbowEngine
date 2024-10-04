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

namespace rhi::vulkan
{
class Instance;

class ValidationLayer final : public IRHIResource
{
public:
    typedef ValidationLayer ThisClass;

    explicit ValidationLayer(Instance* InParentInstance) noexcept : vulkan_instance_(InParentInstance) {}

    void Initialize();
    void DeInitialize();

    void Destroy() override;

    bool IsValid() const { return static_cast<bool>(debug_messenger_callback_); }

#if ELBOW_DEBUG
    static constexpr bool sEnableValidationLayer = true;
#else
    static constexpr bool sEnableValidationLayer = false;
#endif
    static inline TArray<const char*> gValidationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    static constexpr bool                IsEnable() noexcept { return sEnableValidationLayer; }
    static constexpr TArray<const char*> GetValidationLayerNames() noexcept { return gValidationLayers; }

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
        VkDebugUtilsMessageSeverityFlagBitsEXT InMessageSeverity, VkDebugUtilsMessageTypeFlagsEXT InMessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData
    );

    [[nodiscard]] vk::DebugUtilsMessengerEXT GetDebugMessenger() const { return debug_messenger_callback_; }
    ValidationLayer&                         SetAttachedVulkanInstance(Instance* InInstance) noexcept;


private:
    vk::DebugUtilsMessengerEXT debug_messenger_callback_;

    // 验证层附加的Instance对象
    Instance* vulkan_instance_ = nullptr;
};
}
