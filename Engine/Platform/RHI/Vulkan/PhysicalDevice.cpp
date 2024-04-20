/**
 * @file PhysicalDevice.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "PhysicalDevice.h"

#include "Instance.h"
RHI_VULKAN_NAMESPACE_BEGIN


UniquePtr<PhysicalDevice>
PhysicalDevice::PickPhysicalDevice(Instance* InAttachedInstance, const Function<bool(const PhysicalDevice&)>& PickFunc) {
    auto        Rtn     = MakeUnique<PhysicalDevice>(InAttachedInstance);
    const Array Devices = InAttachedInstance->EnumeratePhysicalDevices();
    for (const auto& Device: Devices) {
        PhysicalDevice TempDevice(InAttachedInstance);
        TempDevice.SetVulkanPhysicalDevice(Device);
        if (PickFunc(TempDevice)) {
            Rtn->SetVulkanPhysicalDevice(Device);
            break;
        }
    }
    if (!Rtn->IsValid()) throw VulkanException(L"PhysicalDevice::PickPhysicalDevice: 未找到合适的物理设备");

    return Rtn;
}

bool PhysicalDevice::IsDeviceSuitable(const PhysicalDevice& InDevice) {
    // 检查扩展支持
    if (!InDevice.CheckExtensionSupport(sDeviceRequiredExtensions)) return false;
    // 检查交换链支持
    const auto SwapChainSupport = InDevice.QuerySwapChainSupport();
    if (SwapChainSupport.Formats.empty() || SwapChainSupport.PresentModes.empty()) return false;
    // 检查队列族支持
    const QueueFamilyIndices Indices = InDevice.FindQueueFamilyIndices();
    if (!Indices.IsValid()) return false;
    // 检查设备特性
    const auto& DeviceFeatures = InDevice.GetFeatures();
    const auto& DeviceProps    = InDevice.GetProperties();
    return DeviceProps.deviceType == vk::PhysicalDeviceType::eDiscreteGpu &&   // 独显
           DeviceFeatures.geometryShader &&                                    // 支持几何着色器
           DeviceFeatures.samplerAnisotropy;                                   // 支持各向异性采样
}

PhysicalDevice& PhysicalDevice::SetVulkanPhysicalDevice(const vk::PhysicalDevice InDevice) {
    mDeviceHandle = InDevice;
    return *this;
}

QueueFamilyIndices PhysicalDevice::FindQueueFamilyIndices() const {
    THROW_IF_NOT(VULKAN_CHECK_PTR(mAttachedInstance), L"Physical::FindQueueFamilyIndices: 查找队列族索引时mAttachedInstance无效");
    const Array        QueueFamilies = mDeviceHandle.getQueueFamilyProperties();
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
    const Array     Extensions           = mDeviceHandle.enumerateDeviceExtensionProperties();
    Set<AnsiString> MyRequiredExtensions = RequiredExtensions;
    for (const auto& Extension: Extensions) {
        MyRequiredExtensions.erase(Extension.extensionName);
    }
    return MyRequiredExtensions.empty();
}

PhysicalDevice::SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport() const {
    THROW_IF_NOT(VULKAN_CHECK_PTR(mAttachedInstance), L"Physical::FindQueueFamilyIndices: 查询交换链支持情况mAttachedInstance无效");
    SwapChainSupportDetails RtnDetails;
    RtnDetails.Capabilities = mDeviceHandle.getSurfaceCapabilitiesKHR(mAttachedInstance->GetSurfaceHandle());
    RtnDetails.Formats      = mDeviceHandle.getSurfaceFormatsKHR(mAttachedInstance->GetSurfaceHandle());
    RtnDetails.PresentModes = mDeviceHandle.getSurfacePresentModesKHR(mAttachedInstance->GetSurfaceHandle());
    return RtnDetails;
}

RHI_VULKAN_NAMESPACE_END
