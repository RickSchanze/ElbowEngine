/**
 * @file LogicalDevice.h
 * @author Echo 
 * @Date 24-4-21
 * @brief 
 */

#pragma once
#include "Interface/IRHIResource.h"
#include "vulkan/vulkan.hpp"
#include "VulkanCommon.h"

namespace RHI::Vulkan {
class Instance;
}
RHI_VULKAN_NAMESPACE_BEGIN

class LogicalDevice final : public IRHIResource {
public:
    LogicalDevice() = default;
    explicit LogicalDevice(const vk::Device InDevice) : mLogicalDeviceHandle(InDevice) {}
    void     Initialize() override;
    void     Finalize() override;

private:
    vk::Device mLogicalDeviceHandle = VK_NULL_HANDLE;
};

RHI_VULKAN_NAMESPACE_END
