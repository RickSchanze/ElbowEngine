/**
 * @file Instance.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once
#include "Interface/IRHIResource.h"
#include "PhysicalDevice.h"
#include "ValidationLayer.h"
#include "VulkanCommon.h"

#include <vulkan/vulkan.hpp>

RHI_VULKAN_NAMESPACE_BEGIN

class SurfaceBase : public IRHIResource {
public:
    explicit SurfaceBase(Instance* InParentInstance) : mAttachedInstanceHandle(InParentInstance) {}

    ~SurfaceBase() override = default;

    [[nodiscard]] Instance&      GetInstanceHandle() const { return *mAttachedInstanceHandle; }
    SurfaceBase&                 SetInstanceHandle(Instance* InInstanceHandle);
    [[nodiscard]] vk::SurfaceKHR GetSurfaceHandle() const { return mSurfaceHandle; }
    SurfaceBase&                 SetSurfaceHandle(vk::SurfaceKHR InSurfaceHandle);

    explicit operator vk::SurfaceKHR() const { return mSurfaceHandle; }

    /** 此函数必修初始化mSurface */
    void Initialize() override INTERFACE_METHOD;
    void Finialize() override;

    [[nodiscard]] bool IsValid() const override { return static_cast<bool>(mSurfaceHandle); }

protected:
    vk::SurfaceKHR mSurfaceHandle;
    Instance*      mAttachedInstanceHandle = nullptr;
};

class Instance final : public IRHIResource {
public:
    Instance();

    void Initialize() override;
    void Finialize() override;

    Instance& SetSurface(UniquePtr<SurfaceBase> InSurface);

    // clang-format off
    [[nodiscard]] Array<vk::PhysicalDevice> EnumeratePhysicalDevices() const;
    [[nodiscard]] bool IsValid() const override { return static_cast<bool>(mVulkanInstanceHandle); }
    [[nodiscard]] vk::Instance GetVulkanInstanceHandle() const { return mVulkanInstanceHandle; }
    [[nodiscard]] const vk::DispatchLoaderDynamic& GetDynamicDispatcher() const;
    [[nodiscard]] vk::SurfaceKHR GetSurfaceHandle() const {return mSurface->GetSurfaceHandle();}

    Instance& SetInstanceCreateInfo(const vk::InstanceCreateInfo &InCreateInfo) { mInstanceCreateInfo = InCreateInfo; return *this; }
    // clang-format on

    /**
     * 选择一个合适的PhysicalDevice返回
     * @return
     */
    UniquePtr<PhysicalDevice> PickPhysicalDevice();

protected:
    void InitializeSurface();

private:
    vk::Instance               mVulkanInstanceHandle;
    // 验证层
    UniquePtr<ValidationLayer> mValidationLayer;
    // 窗口表面
    UniquePtr<SurfaceBase>     mSurface;
    // 动态加载各种函数用
    vk::DispatchLoaderDynamic  mDynamicDispatcher;
    // 实例创建信息
    vk::InstanceCreateInfo     mInstanceCreateInfo;
};

RHI_VULKAN_NAMESPACE_END
