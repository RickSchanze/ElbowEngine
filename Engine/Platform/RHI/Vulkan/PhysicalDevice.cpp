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


TUniquePtr<PhysicalDevice> PhysicalDevice::PickPhysicalDevice(Instance* instance, const TFunction<bool(const PhysicalDevice&)>& pick_func)
{
    auto         Rtn     = MakeUnique<PhysicalDevice>(instance);
    const TArray Devices = instance->EnumeratePhysicalDevices();
    for (const auto& Device: Devices)
    {
        PhysicalDevice TempDevice(instance);
        TempDevice.SetVulkanPhysicalDevice(Device);
        if (pick_func(TempDevice))
        {
            Rtn->SetVulkanPhysicalDevice(Device);
            break;
        }
    }
    if (!Rtn->IsValid()) throw VulkanException(L"PhysicalDevice::PickPhysicalDevice: 未找到合适的物理设备");

    return Rtn;
}

bool PhysicalDevice::IsDeviceSuitable(const PhysicalDevice& device)
{
    // 检查扩展支持
    if (!device.CheckExtensionSupport(s_device_required_extensions)) return false;
    // 检查交换链支持
    const auto swap_chain_support = device.QuerySwapChainSupport();
    if (swap_chain_support.formats.empty() || swap_chain_support.present_modes.empty()) return false;
    // 检查队列族支持
    const QueueFamilyIndices index = device.FindQueueFamilyIndices();
    if (!index.IsValid()) return false;
    // 检查设备特性
    const auto& device_features = device.GetFeatures();
    const auto& device_props    = device.GetProperties();
    return device_props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu &&   // 独显
           device_features.geometryShader &&                                    // 支持几何着色器
           device_features.samplerAnisotropy;                                   // 支持各向异性采样
}

PhysicalDevice& PhysicalDevice::SetVulkanPhysicalDevice(const vk::PhysicalDevice device)
{
    handle_ = device;
    return *this;
}

QueueFamilyIndices PhysicalDevice::FindQueueFamilyIndices() const
{
    THROW_IF_NOT(VULKAN_CHECK_PTR(instance_), L"Physical::FindQueueFamilyIndices: 查找队列族索引时mAttachedInstance无效");
    const TArray       QueueFamilies = handle_.getQueueFamilyProperties();
    // 查找需要的队列族索引
    QueueFamilyIndices Rtn{};
    for (int i = 0; i < QueueFamilies.size(); i++)
    {
        auto& QueueFamily = QueueFamilies[i];
        // 检查支不支持图形管线
        if (QueueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            Rtn.graphics_family = i;
        }
        // 检查支不支持Surface
        if (handle_.getSurfaceSupportKHR(i, instance_->GetSurfaceHandle()))
        {
            Rtn.present_family = i;
        }
        if (Rtn.IsValid())
        {
            return Rtn;
        }
    }
    return {};
}

bool PhysicalDevice::CheckExtensionSupport(const TArray<const char*, std::allocator<const char*>>& required_extensions) const
{
    const TArray      extensions             = handle_.enumerateDeviceExtensionProperties();
    TSet<const char*> my_required_extensions = {required_extensions.begin(), required_extensions.end()};
    for (const auto& extension: extensions)
    {
        my_required_extensions.erase(extension.extensionName);
    }
    return !my_required_extensions.empty();
}

PhysicalDevice::SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport() const
{
    THROW_IF_NOT(VULKAN_CHECK_PTR(instance_), L"Physical::FindQueueFamilyIndices: 查询交换链支持情况mAttachedInstance无效");
    SwapChainSupportDetails RtnDetails;
    RtnDetails.capabilities  = handle_.getSurfaceCapabilitiesKHR(instance_->GetSurfaceHandle());
    RtnDetails.formats       = handle_.getSurfaceFormatsKHR(instance_->GetSurfaceHandle());
    RtnDetails.present_modes = handle_.getSurfacePresentModesKHR(instance_->GetSurfaceHandle());
    return RtnDetails;
}

vk::Format
PhysicalDevice::FindSupportFormat(const TArray<vk::Format>& candidates, const vk::ImageTiling tiling, const vk::FormatFeatureFlagBits features) const
{
    for (const vk::Format& Format: candidates)
    {
        const auto Props = handle_.getFormatProperties(Format);
        // 图形布局经过优化
        if (tiling == vk::ImageTiling::eOptimal && (Props.optimalTilingFeatures & features) == features)
        {
            return Format;
        }
        // if (InTiling == vk::ImageTiling::eLinear && (Props.linearTilingFeatures & InFeatures) == InFeatures) {
        //     // 图像在内部行主序存储 导致要访问下一列数据则需要跨行 不好
        //     return Format;
        // }
    }
    throw VulkanException(L"PhysicalDevice::FindSupportFormat: 未找到支持的格式");
}

uint32_t PhysicalDevice::FindMemoryType(const uint32_t type_filter, const vk::MemoryPropertyFlags properties) const
{
    const auto MemProperties = handle_.getMemoryProperties();
    for (uint32_t i = 0; i < MemProperties.memoryTypeCount; i++)
    {
        if ((type_filter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw VulkanException(L"PhysicalDevice::FindMemoryType: 未找到合适的内存类型");
}

TUniquePtr<LogicalDevice> PhysicalDevice::CreateLogicalDeviceUnique()
{
    const auto LogicalDeviceHandle = CreateLogicalDeviceHandle();
    return LogicalDevice::CreateUnique(LogicalDeviceHandle, *this);
}

vk::Device PhysicalDevice::CreateLogicalDeviceHandle() const
{
    QueueFamilyIndices                indices = FindQueueFamilyIndices();
    TArray<vk::DeviceQueueCreateInfo> queue_create_infos;
    TSet<uint32_t>                    unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};
    float                             queue_priority        = 1.0f;
    for (uint32_t QueueFamily: unique_queue_families)
    {
        vk::DeviceQueueCreateInfo queue_create_info{};
        queue_create_info
            .setQueueFamilyIndex(QueueFamily)        // 队列族索引
            .setQueueCount(1)                        // 队列数量
            .setPQueuePriorities(&queue_priority);   // 队列优先级 即使只有一个队列也需要指定
        queue_create_infos.push_back(queue_create_info);
    }
    // 指定要使用的设备特性
    vk::PhysicalDeviceRobustness2FeaturesEXT robustness2_features;
    robustness2_features.nullDescriptor = true;
    vk::PhysicalDeviceFeatures device_features{};
    device_features.setSamplerAnisotropy(true);   // 开启采样器各向异性过滤支持
    device_features.setGeometryShader(true);

    // 创建逻辑设备
    vk::DeviceCreateInfo device_info{};
    device_info
        .setQueueCreateInfos(queue_create_infos)                   // 设置队列创建信息
        .setPEnabledFeatures(&device_features)                     // 设置要求的设备特性
        .setPEnabledExtensionNames(s_device_required_extensions)   // 设置启用的扩展名
        .setPNext(&robustness2_features);
    // 校验层
    auto validation_layers = ValidationLayer::GetValidationLayerNames();
    if (ValidationLayer::IsEnable())
    {
        device_info.setEnabledLayerCount(validation_layers.size()).setPpEnabledLayerNames(validation_layers.data());
    }
    auto logical_device_handle = handle_.createDevice(device_info);
    return logical_device_handle;
}

RHI_VULKAN_NAMESPACE_END
