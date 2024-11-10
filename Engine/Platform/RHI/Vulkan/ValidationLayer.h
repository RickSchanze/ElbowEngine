/**
 * @file ValidationLayer.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once
#include "Base/CoreTypeDef.h"
#include "Interface/IRHIResource.h"
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
    void DeInitialize() const;

    void Destroy() override;

    bool IsValid() const { return static_cast<bool>(debug_messenger_callback_); }

#if ELBOW_DEBUG
    static constexpr bool sEnableValidationLayer = true;
#else
    static constexpr bool sEnableValidationLayer = false;
#endif
    static inline core::Array<const char*> gValidationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    static constexpr bool                     IsEnable() noexcept { return sEnableValidationLayer; }
    static constexpr core::Array<const char*> GetValidationLayerNames() noexcept { return gValidationLayers; }

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data
    );

    [[nodiscard]] vk::DebugUtilsMessengerEXT GetDebugMessenger() const { return debug_messenger_callback_; }
    ValidationLayer&                         SetAttachedVulkanInstance(Instance* InInstance) noexcept;


private:
    vk::DebugUtilsMessengerEXT debug_messenger_callback_;

    // 验证层附加的Instance对象
    Instance* vulkan_instance_ = nullptr;
};
}
