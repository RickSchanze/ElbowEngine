/**
 * @file Instance.cpp
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#include "Instance.h"

#include "CoreGlobal.h"
#include "PlatformLogcat.h"

namespace rhi::vulkan
{
SurfaceBase& SurfaceBase::SetInstanceHandle(Instance* InInstanceHandle) {
    mAttachedInstanceHandle = InInstanceHandle;
    return *this;
}

SurfaceBase& SurfaceBase::SetSurfaceHandle(vk::SurfaceKHR InSurfaceHandle) {
    mSurfaceHandle = InSurfaceHandle;
    return *this;
}

void SurfaceBase::DeInitialize() {
    if (mAttachedInstanceHandle->IsValid()) {
        mAttachedInstanceHandle->GetHandle().destroySurfaceKHR(mSurfaceHandle);
        mSurfaceHandle = VK_NULL_HANDLE;
        LOGGER.Info(logcat::Platform_RHI_Vulkan, "Surface Destroyed");
    }
}

void SurfaceBase::Destroy() {
    DeInitialize();
}

Instance::Instance() {
    vulkan_instance_handle_ = VK_NULL_HANDLE;
}

void Instance::Initialize() {
    if (IsValid()) return;
    vulkan_instance_handle_ = createInstance(mInstanceCreateInfo);
    dynamic_dispatcher_    = {vulkan_instance_handle_, vkGetInstanceProcAddr};
    // 初始化验证层
    validation_layer_      = core::MakeUnique<ValidationLayer>(this);
    validation_layer_->Initialize();
    // 初始化窗口表面
    InitializeSurface();
}

void Instance::DeInitialize() {
    if (!IsValid()) return;
    surface_->DeInitialize();
    validation_layer_->DeInitialize();
    vulkan_instance_handle_.destroy();
    vulkan_instance_handle_ = VK_NULL_HANDLE;
}

void Instance::Destroy() {
    DeInitialize();
}

const vk::DispatchLoaderDynamic& Instance::GetDynamicDispatcher() const {
    return dynamic_dispatcher_;
}

Instance& Instance::SetSurface(core::UniquePtr<SurfaceBase> InSurface) {
    surface_ = Move(InSurface);
    surface_->SetInstanceHandle(this);
    return *this;
}

core::Array<vk::PhysicalDevice> Instance::EnumeratePhysicalDevices() const {
    return vulkan_instance_handle_.enumeratePhysicalDevices();
}

void Instance::InitializeSurface() {
    surface_->SetInstanceHandle(this);
    surface_->Initialize();
    LOGGER.Info(logcat::Platform_RHI_Vulkan, "Surface initialized");
}

core::UniquePtr<PhysicalDevice> Instance::PickPhysicalDevice() {
    auto PhysicalDevice = PhysicalDevice::PickPhysicalDevice(this);
    return PhysicalDevice;
}
}
