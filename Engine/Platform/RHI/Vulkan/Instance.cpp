/**
 * @file Instance.cpp
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#include "Instance.h"

#include "CoreGlobal.h"
#include "Utils/StringUtils.h"

RHI_VULKAN_NAMESPACE_BEGIN

SurfaceBase& SurfaceBase::SetInstanceHandle(Instance* InInstanceHandle) {
    mAttachedInstanceHandle = InInstanceHandle;
    return *this;
}

SurfaceBase& SurfaceBase::SetSurfaceHandle(vk::SurfaceKHR InSurfaceHandle) {
    mSurfaceHandle = InSurfaceHandle;
    return *this;
}

void SurfaceBase::Finialize() {
    if (mAttachedInstanceHandle->IsValid()) {
        mAttachedInstanceHandle->GetVulkanInstanceHandle().destroySurfaceKHR(mSurfaceHandle);
        mSurfaceHandle = VK_NULL_HANDLE;
        LOG_INFO_CATEGORY(Vulkan, L"Surface清理完成");
    }
}

Instance::Instance() {
    mVulkanInstanceHandle = VK_NULL_HANDLE;
}

void Instance::Initialize() {
    if (IsValid()) return;
    mVulkanInstanceHandle = createInstance(mInstanceCreateInfo);
    mDynamicDispatcher    = {mVulkanInstanceHandle, vkGetInstanceProcAddr};

    // 初始化验证层
    mValidationLayer = MakeUnique<ValidationLayer>(this);
    mValidationLayer->Initialize();
    // 初始化窗口表面
    InitializeSurface();

    // 选择物理设备
    PickPhysicalDevice();
}

void Instance::Finialize() {
    if (!IsValid()) return;
    mSurface->Finialize();
    mValidationLayer->Finialize();
    mVulkanInstanceHandle.destroy();
    mVulkanInstanceHandle = VK_NULL_HANDLE;
}

const vk::DispatchLoaderDynamic& Instance::GetDynamicDispatcher() const {
    return mDynamicDispatcher;
}

Instance& Instance::SetSurface(UniquePtr<SurfaceBase> InSurface) {
    mSurface = Move(InSurface);
    return *this;
}

Array<vk::PhysicalDevice> Instance::EnumeratePhysicalDevices() const {
    return mVulkanInstanceHandle.enumeratePhysicalDevices();
}

SharedPtr<LogicalDevice> Instance::CreateLogicalDeviceShared() const {
    return mPhysicalDevice->CreateLogicalDeviceShared();
}

UniquePtr<LogicalDevice> Instance::CreateLogicalDeviceUnique() const{
    return mPhysicalDevice->CreateLogicalDeviceUnique();
}

void Instance::InitializeSurface() const {
    mSurface->Initialize();
    LOG_INFO_CATEGORY(Vulkan, L"Surface初始化完成");
}

void Instance::PickPhysicalDevice() {
    mPhysicalDevice       = PhysicalDevice::PickPhysicalDevice(this);
    const auto Properties = mPhysicalDevice->GetProperties();
    auto       Name       = StringUtils::FromAnsiString(Properties.deviceName);
    LOG_INFO_CATEGORY(Vulkan, L"物理设备选择完成. 选用: {}", Name);
}

RHI_VULKAN_NAMESPACE_END
