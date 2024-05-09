/**
 * @file PhysicalDevice.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "PhysicalDevice.h"

#include "CoreGlobal.h"
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

bool PhysicalDevice::CheckExtensionSupport(const Array<const char*, std::allocator<const char*>>& RequiredExtensions) const {
    const Array      Extensions           = mDeviceHandle.enumerateDeviceExtensionProperties();
    Set<const char*> MyRequiredExtensions = {RequiredExtensions.begin(), RequiredExtensions.end()};
    for (const auto& Extension: Extensions) {
        MyRequiredExtensions.erase(Extension.extensionName);
    }
    return !MyRequiredExtensions.empty();
}

PhysicalDevice::SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport() const {
    THROW_IF_NOT(VULKAN_CHECK_PTR(mAttachedInstance), L"Physical::FindQueueFamilyIndices: 查询交换链支持情况mAttachedInstance无效");
    SwapChainSupportDetails RtnDetails;
    RtnDetails.Capabilities = mDeviceHandle.getSurfaceCapabilitiesKHR(mAttachedInstance->GetSurfaceHandle());
    RtnDetails.Formats      = mDeviceHandle.getSurfaceFormatsKHR(mAttachedInstance->GetSurfaceHandle());
    RtnDetails.PresentModes = mDeviceHandle.getSurfacePresentModesKHR(mAttachedInstance->GetSurfaceHandle());
    return RtnDetails;
}

vk::Format PhysicalDevice::FindSupportFormat(
    const Array<vk::Format>& InCandidates, const vk::ImageTiling InTiling, const vk::FormatFeatureFlagBits InFeatures
) const {
    for (const vk::Format& Format: InCandidates) {
        const auto Props = mDeviceHandle.getFormatProperties(Format);
        // 图形布局经过优化
        if (InTiling == vk::ImageTiling::eOptimal && (Props.optimalTilingFeatures & InFeatures) == InFeatures) {
            return Format;
        }
        // if (InTiling == vk::ImageTiling::eLinear && (Props.linearTilingFeatures & InFeatures) == InFeatures) {
        //     // 图像在内部行主序存储 导致要访问下一列数据则需要跨行 不好
        //     return Format;
        // }
    }
    throw VulkanException(L"PhysicalDevice::FindSupportFormat: 未找到支持的格式");
}

uint32 PhysicalDevice::FindMemoryType(const uint32 InTypeFilter, const vk::MemoryPropertyFlags InProperties) const {
    const auto MemProperties = mDeviceHandle.getMemoryProperties();
    for (uint32 i = 0; i < MemProperties.memoryTypeCount; i++) {
        if ((InTypeFilter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & InProperties) == InProperties) {
            return i;
        }
    }
    throw VulkanException(L"PhysicalDevice::FindMemoryType: 未找到合适的内存类型");
}

UniquePtr<LogicalDevice> PhysicalDevice::CreateLogicalDeviceUnique() {
    const auto LogicalDeviceHandle = CreateLogicalDeviceHandle();
    return LogicalDevice::CreateUnique(LogicalDeviceHandle, *this);
}

vk::Device PhysicalDevice::CreateLogicalDeviceHandle() const {
    QueueFamilyIndices               Indices = FindQueueFamilyIndices();
    Array<vk::DeviceQueueCreateInfo> QueueCreateInfos;
    Set<uint32_t>                    UniqueQueueFamilies = {Indices.GraphicsFamily.value(), Indices.PresentFamily.value()};
    float                            QueuePriority       = 1.0f;
    for (uint32_t QueueFamily: UniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo QueueCreateInfo{};
        QueueCreateInfo
            .setQueueFamilyIndex(QueueFamily)       // 队列族索引
            .setQueueCount(1)                       // 队列数量
            .setPQueuePriorities(&QueuePriority);   // 队列优先级 即使只有一个队列也需要指定
        QueueCreateInfos.push_back(QueueCreateInfo);
    }
    // 指定要使用的设备特性
    vk::PhysicalDeviceFeatures DeviceFeatures{};
    DeviceFeatures.setSamplerAnisotropy(true);   // 开启采样器各向异性过滤支持
    DeviceFeatures.setGeometryShader(true);

    // 创建逻辑设备
    vk::DeviceCreateInfo DeviceInfo{};
    DeviceInfo
        .setQueueCreateInfos(QueueCreateInfos)                   // 设置队列创建信息
        .setPEnabledFeatures(&DeviceFeatures)                    // 设置要求的设备特性
        .setPEnabledExtensionNames(sDeviceRequiredExtensions);   // 设置启用的扩展名
    // 校验层
    auto ValidationLayers = ValidationLayer::GetValidationLayerNames();
    if (ValidationLayer::IsEnable()) {
        DeviceInfo.setEnabledLayerCount(ValidationLayers.size()).setPpEnabledLayerNames(ValidationLayers.data());
    }
    auto LogicalDeviceHandle = mDeviceHandle.createDevice(DeviceInfo);
    return LogicalDeviceHandle;
}



RHI_VULKAN_NAMESPACE_END
