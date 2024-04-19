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

void Instance::Initialize() {
    mDynamicDispatcher = {mVulkanInstance, vkGetInstanceProcAddr};
}

void Instance::Finalize() {

}

RHI_VULKAN_NAMESPACE_END

