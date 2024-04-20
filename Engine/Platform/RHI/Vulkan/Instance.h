/**
 * @file Instance.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once
#include "Interface/IRHIResource.h"
#include "ValidationLayer.h"
#include "VulkanCommon.h"

#include <vulkan/vulkan.hpp>

RHI_VULKAN_NAMESPACE_BEGIN

class Instance : public IRHIResource {
public:
             Instance();
             Instance(const Instance&) = default;
    explicit Instance(const vk::InstanceCreateInfo& InCreateInfo);

    void Initialize() override;
    void Finalize() override;

    [[nodiscard]] vk::Instance GetVulkanInstance() const { return mVulkanInstance; }
    [[nodiscard]] const vk::DispatchLoaderDynamic& GetDynamicDispatcher() const {
        return mDynamicDispatcher;
    }

    [[nodiscard]] bool IsValid() const { return static_cast<bool>(mVulkanInstance); }

private:
    vk::Instance              mVulkanInstance;
    // 验证层
    ValidationLayer           mValidationLayer;
    // 动态加载各种函数用
    vk::DispatchLoaderDynamic mDynamicDispatcher;
};

RHI_VULKAN_NAMESPACE_END
