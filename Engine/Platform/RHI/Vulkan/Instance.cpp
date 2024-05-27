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
        mAttachedInstanceHandle->GetHandle().destroySurfaceKHR(mSurfaceHandle);
        mSurfaceHandle = VK_NULL_HANDLE;
        LOG_INFO_CATEGORY(Vulkan, L"Surface清理完成");
    }
}

void SurfaceBase::Destroy() {
    Finialize();
}

Instance::Instance() {
    mVulkanInstanceHandle = VK_NULL_HANDLE;
}

void Instance::Initialize() {
    if (IsValid()) return;
    mVulkanInstanceHandle = createInstance(mInstanceCreateInfo);
    mDynamicDispatcher    = {mVulkanInstanceHandle, vkGetInstanceProcAddr};
    // 初始化验证层
    mValidationLayer      = MakeUnique<ValidationLayer>(this);
    mValidationLayer->Initialize();
    // 初始化窗口表面
    InitializeSurface();
}

void Instance::Finialize() {
    if (!IsValid()) return;
    mSurface->Finialize();
    mValidationLayer->Finialize();
    mVulkanInstanceHandle.destroy();
    mVulkanInstanceHandle = VK_NULL_HANDLE;
}

void Instance::Destroy(){
    Finialize();
}

const vk::DispatchLoaderDynamic& Instance::GetDynamicDispatcher() const {
    return mDynamicDispatcher;
}

Instance& Instance::SetSurface(TUniquePtr<SurfaceBase> InSurface) {
    mSurface = Move(InSurface);
    mSurface->SetInstanceHandle(this);
    return *this;
}

TArray<vk::PhysicalDevice> Instance::EnumeratePhysicalDevices() const {
    return mVulkanInstanceHandle.enumeratePhysicalDevices();
}

void Instance::InitializeSurface() {
    mSurface->SetInstanceHandle(this);
    mSurface->Initialize();
    LOG_INFO_CATEGORY(Vulkan, L"Surface初始化完成");
}

TUniquePtr<PhysicalDevice> Instance::PickPhysicalDevice() {
    auto PhysicalDevice = PhysicalDevice::PickPhysicalDevice(this);
    return PhysicalDevice;
}

RHI_VULKAN_NAMESPACE_END
