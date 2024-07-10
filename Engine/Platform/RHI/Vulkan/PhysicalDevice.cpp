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


TUniquePtr<PhysicalDevice>
PhysicalDevice::PickPhysicalDevice(Instance* instance, const TFunction<bool(const PhysicalDevice&)>& pick_func) {
    auto        Rtn     = MakeUnique<PhysicalDevice>(instance);
    const TArray Devices = instance->EnumeratePhysicalDevices();
    for (const auto& Device: Devices) {
        PhysicalDevice TempDevice(instance);
        TempDevice.SetVulkanPhysicalDevice(Device);
        if (pick_func(TempDevice)) {
            Rtn->SetVulkanPhysicalDevice(Device);
            break;
        }
    }
    if (!Rtn->IsValid()) throw VulkanException(L"PhysicalDevice::PickPhysicalDevice: 未找到合适的物理设备");

    return Rtn;
}

bool PhysicalDevice::IsDeviceSuitable(const PhysicalDevice& InDevice) {
    // 检查扩展支持
    if (!InDevice.CheckExtensionSupport(s_device_required_extensions)) return false;
    // 检查交换链支持
    const auto SwapChainSupport = InDevice.QuerySwapChainSupport();
    if (SwapChainSupport.formats.empty() || SwapChainSupport.present_modes.empty()) return false;
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

PhysicalDevice& PhysicalDevice::SetVulkanPhysicalDevice(const vk::PhysicalDevice device) {
    handle_ = device;
    return *this;
}

QueueFamilyIndices PhysicalDevice::FindQueueFamilyIndices() const {
    THROW_IF_NOT(VULKAN_CHECK_PTR(instance_), L"Physical::FindQueueFamilyIndices: 查找队列族索引时mAttachedInstance无效");
    const TArray        QueueFamilies = handle_.getQueueFamilyProperties();
    // 查找需要的队列族索引
    QueueFamilyIndices Rtn{};
    for (int i = 0; i < QueueFamilies.size(); i++) {
        auto& QueueFamily = QueueFamilies[i];
        // 检查支不支持图形管线
        if (QueueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            Rtn.graphics_family = i;
        }
        // 检查支不支持Surface
        if (handle_.getSurfaceSupportKHR(i, instance_->GetSurfaceHandle())) {
            Rtn.present_family = i;
        }
        if (Rtn.IsValid()) {
            return Rtn;
        }
    }
    return {};
}

bool PhysicalDevice::CheckExtensionSupport(const TArray<const char*, std::allocator<const char*>>& required_extensions) const {
    const TArray      Extensions           = handle_.enumerateDeviceExtensionProperties();
    TSet<const char*> MyRequiredExtensions = {required_extensions.begin(), required_extensions.end()};
    for (const auto& Extension: Extensions) {
        MyRequiredExtensions.erase(Extension.extensionName);
    }
    return !MyRequiredExtensions.empty();
}

PhysicalDevice::SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport() const {
    THROW_IF_NOT(VULKAN_CHECK_PTR(instance_), L"Physical::FindQueueFamilyIndices: 查询交换链支持情况mAttachedInstance无效");
    SwapChainSupportDetails RtnDetails;
    RtnDetails.capabilities = handle_.getSurfaceCapabilitiesKHR(instance_->GetSurfaceHandle());
    RtnDetails.formats      = handle_.getSurfaceFormatsKHR(instance_->GetSurfaceHandle());
    RtnDetails.present_modes = handle_.getSurfacePresentModesKHR(instance_->GetSurfaceHandle());
    return RtnDetails;
}

vk::Format PhysicalDevice::FindSupportFormat(
    const TArray<vk::Format>& candidates, const vk::ImageTiling tiling, const vk::FormatFeatureFlagBits features
) const {
    for (const vk::Format& Format: candidates) {
        const auto Props = handle_.getFormatProperties(Format);
        // 图形布局经过优化
        if (tiling == vk::ImageTiling::eOptimal && (Props.optimalTilingFeatures & features) == features) {
            return Format;
        }
        // if (InTiling == vk::ImageTiling::eLinear && (Props.linearTilingFeatures & InFeatures) == InFeatures) {
        //     // 图像在内部行主序存储 导致要访问下一列数据则需要跨行 不好
        //     return Format;
        // }
    }
    throw VulkanException(L"PhysicalDevice::FindSupportFormat: 未找到支持的格式");
}

uint32_t PhysicalDevice::FindMemoryType(const uint32_t type_filter, const vk::MemoryPropertyFlags properties) const {
    const auto MemProperties = handle_.getMemoryProperties();
    for (uint32_t i = 0; i < MemProperties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw VulkanException(L"PhysicalDevice::FindMemoryType: 未找到合适的内存类型");
}

TUniquePtr<LogicalDevice> PhysicalDevice::CreateLogicalDeviceUnique() {
    const auto LogicalDeviceHandle = CreateLogicalDeviceHandle();
    return LogicalDevice::CreateUnique(LogicalDeviceHandle, *this);
}

vk::Device PhysicalDevice::CreateLogicalDeviceHandle() const {
    QueueFamilyIndices               Indices = FindQueueFamilyIndices();
    TArray<vk::DeviceQueueCreateInfo> QueueCreateInfos;
    TSet<uint32_t>                    UniqueQueueFamilies = {Indices.graphics_family.value(), Indices.present_family.value()};
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
        .setPEnabledExtensionNames(s_device_required_extensions);   // 设置启用的扩展名
    // 校验层
    auto ValidationLayers = ValidationLayer::GetValidationLayerNames();
    if (ValidationLayer::IsEnable()) {
        DeviceInfo.setEnabledLayerCount(ValidationLayers.size()).setPpEnabledLayerNames(ValidationLayers.data());
    }
    auto LogicalDeviceHandle = handle_.createDevice(DeviceInfo);
    return LogicalDeviceHandle;
}



RHI_VULKAN_NAMESPACE_END
