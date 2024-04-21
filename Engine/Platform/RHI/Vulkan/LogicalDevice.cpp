/**
 * @file LogicalDevice.cpp
 * @author Echo 
 * @Date 24-4-21
 * @brief 
 */

#include "LogicalDevice.h"

#include "CoreGlobal.h"

void RHI::Vulkan::LogicalDevice::Initialize() {
    LOG_INFO_CATEGORY(VK_NULL_HANDLE, L"逻辑设备初始化完成");
}

void RHI::Vulkan::LogicalDevice::Finalize() {
    mLogicalDeviceHandle.destroy();
    mLogicalDeviceHandle = VK_NULL_HANDLE;
    LOG_INFO_CATEGORY(VK_NULL_HANDLE, L"逻辑设备清理完成");
}
