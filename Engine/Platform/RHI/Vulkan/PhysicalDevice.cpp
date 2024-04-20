/**
 * @file PhysicalDevice.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "PhysicalDevice.h"

#include "Instance.h"
RHI_VULKAN_NAMESPACE_BEGIN


UniquePtr<PhysicalDevice> PhysicalDevice::PickPhysicalDevice(
    Instance* InAttachedInstance, const Function<bool(const PhysicalDevice&)>& PickFunc
) {
    auto Rtn      = MakeUnique<PhysicalDevice>(InAttachedInstance);
    Array Devices = InAttachedInstance->EnumeratePhysicalDevices();
    return Rtn;
}

bool PhysicalDevice::IsDeviceSuitable(const PhysicalDevice& InDevice) {
    if (!InDevice.CheckExtensionSupport(sDeviceRequiredExtensions)) return false;
    const QueueFamilyIndices Indices = InDevice.FindQueueFamilyIndices();
    if (!Indices.IsValid()) return false;
    return true;
}

PhysicalDevice& PhysicalDevice::SetVulkanPhysicalDevice(const vk::PhysicalDevice InDevice) {
    mDeviceHandle = InDevice;
    return *this;
}

QueueFamilyIndices PhysicalDevice::FindQueueFamilyIndices() const {
    THROW_IF_NOT(
        VULKAN_CHECK_PTR(mAttachedInstance),
        L"Physical::FindQueueFamilyIndices: 查找队列族索引时mAttachedInstance无效"
    );
    const Array QueueFamilies = mDeviceHandle.getQueueFamilyProperties();
    // 查找需要的队列族索引
    QueueFamilyIndices Rtn{};
    for (int i = 0; i < QueueFamilies.size(); i++) {
        auto& QueueFamily = QueueFamilies[i];
        // 检查支不支持图形管线
        if (QueueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            Rtn.GraphicsFamily = i;
        }
        // 检查支不支持Surface
        if (mDeviceHandle.getSurfaceSupportKHR(i, mAttachedInstance->GetSurfaceHandle())) {
            Rtn.PresentFamily = i;
        }
        if (Rtn.IsValid()) {
            return Rtn;
        }
    }
    return {};
}

bool PhysicalDevice::CheckExtensionSupport(const Set<AnsiString>& RequiredExtensions) const {
    const Array Extensions               = mDeviceHandle.enumerateDeviceExtensionProperties();
    Set<AnsiString> MyRequiredExtensions = RequiredExtensions;
    for (const auto& Extension: Extensions) {
        MyRequiredExtensions.erase(Extension.extensionName);
    }
    return MyRequiredExtensions.empty();
}

PhysicalDevice::SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(){
    THROW_IF_NOT(
        VULKAN_CHECK_PTR(mAttachedInstance),
        L"Physical::FindQueueFamilyIndices: 查询交换链支持情况mAttachedInstance无效"
    );
    SwapChainSupportDetails RtnDetails;
    // TODO:
    RtnDetails.Capabilities = mDeviceHandle.getSurfaceCapabilitiesKHR();
}

RHI_VULKAN_NAMESPACE_END
