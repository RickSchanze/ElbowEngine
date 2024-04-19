/**
 * @file ValidationLayer.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once
#include "Instance.h"
#include "Interface/IRHIResource.h"
#include "VulkanCommon.h"

#include <vulkan/vulkan.hpp>

RHI_VULKAN_NAMESPACE_BEGIN

class ValidationLayer final : public IRHIResource {
public:
    void Initialize() override;
    void Finalize() override;

#if ELBOW_DEBUG
    static constexpr bool sEnableValidationLayer = true;
#else
    static constexpr bool sEnableValidationLayer = false;
#endif

private:
    vk::DebugUtilsMessengerEXT mDebugMessengerCallback{};
    // 验证层附加的Instance对象
    Instance                   mVulkanInstance{};
};

RHI_VULKAN_NAMESPACE_END
