/**
 * @file Instance.cpp
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#include "Instance.h"
RHI_VULKAN_NAMESPACE_BEGIN

Instance::Instance() {
    mVulkanInstance = VK_NULL_HANDLE;
}

Instance::Instance(const vk::InstanceCreateInfo& InCreateInfo) {
    mVulkanInstance = vk::createInstance(InCreateInfo);
}

void Instance::Initialize() {
    mDynamicDispatcher = {mVulkanInstance, vkGetInstanceProcAddr};
    mValidationLayer.SetAttachedVulkanInstance(this);
    mValidationLayer.Initialize();
}

void Instance::Finalize() {
    mValidationLayer.Finalize();
    mVulkanInstance.destroy();
    mVulkanInstance = VK_NULL_HANDLE;
}

RHI_VULKAN_NAMESPACE_END
