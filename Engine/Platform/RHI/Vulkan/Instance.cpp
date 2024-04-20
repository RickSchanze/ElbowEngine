/**
 * @file Instance.cpp
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#include "Instance.h"

#include "CoreGlobal.h"
#include "Exception.h"
RHI_VULKAN_NAMESPACE_BEGIN

void SurfaceBase::Finalize() {
    if (mAttachedInstance->IsValid()) {
        mAttachedInstance->GetVulkanInstance().destroySurfaceKHR(mSurface);
        mSurface = VK_NULL_HANDLE;
    }
}

Instance::Instance() {
    mVulkanInstance = VK_NULL_HANDLE;
}

Instance::Instance(const vk::InstanceCreateInfo& InCreateInfo) {
    mVulkanInstance = vk::createInstance(InCreateInfo);
}

void Instance::Initialize() {
    mDynamicDispatcher = {mVulkanInstance, vkGetInstanceProcAddr};
    mValidationLayer   = MakeUnique<ValidationLayer>(this);
    mValidationLayer->Initialize();
    InitializeSurface();
}

void Instance::Finalize() {
    mValidationLayer->Finalize();
    mVulkanInstance.destroy();
    mVulkanInstance = VK_NULL_HANDLE;
}

Instance& Instance::SetSurface(UniquePtr<SurfaceBase> InSurface) {
    mSurface = Move(InSurface);
    return *this;
}

void Instance::InitializeSurface() const {
    if (!mSurface) {
        throw VulkanException(L"mSurface为空,此字段表面必须手动设置");
    }
    mSurface->Initialize();
    LOG_INFO_CATEGORY(Vulkan, L"Surface初始化完成");
}

RHI_VULKAN_NAMESPACE_END
