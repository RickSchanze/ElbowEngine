/**
 * @file Instance.cpp
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#include "Instance.h"
Vulkan::Instance::Instance() {
    mVulkanInstance = VK_NULL_HANDLE;
}

void Vulkan::Instance::Initialize() {
    mDynamicDispatcher = {mVulkanInstance, vkGetInstanceProcAddr};
}

void Vulkan::Instance::Finalize() {

}

