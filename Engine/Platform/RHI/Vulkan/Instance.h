/**
 * @file Instance.h
 * @author Echo 
 * @Date 24-4-19
 * @brief 
 */

#pragma once
#include "Interface/IRHIResource.h"
#include "ValidationLayer.h"
#include "VulkanCommon.h"

#include <vulkan/vulkan.hpp>

RHI_VULKAN_NAMESPACE_BEGIN

class SurfaceBase : public IRHIResource {
public:
    explicit SurfaceBase(Instance* InParentInstance) : mAttachedInstance(InParentInstance) {}

    ~SurfaceBase() override = default;

    [[nodiscard]] vk::SurfaceKHR GetSurface() const { return mSurface; }

    explicit operator vk::SurfaceKHR() const { return mSurface; }

    /** 此函数必修初始化mSurface */
    void Initialize() override INTERFACE_METHOD;
    void Finalize() override;

    [[nodiscard]] bool IsValid() const { return static_cast<bool>(mSurface); }

protected:
    vk::SurfaceKHR mSurface;
    Instance* mAttachedInstance = nullptr;
};

class Instance : public IRHIResource {
public:
    Instance();

    explicit Instance(const vk::InstanceCreateInfo& InCreateInfo);

    void Initialize() override;
    void Finalize() override;

    Instance& SetSurface(UniquePtr<SurfaceBase> InSurface);

    [[nodiscard]] Array<vk::PhysicalDevice> EnumeratePhysicalDevices() const;
    [[nodiscard]] bool IsValid() const { return static_cast<bool>(mVulkanInstance); }
    [[nodiscard]] vk::Instance GetVulkanInstance() const { return mVulkanInstance; }
    [[nodiscard]] const vk::DispatchLoaderDynamic& GetDynamicDispatcher() const;
    [[nodiscard]] vk::SurfaceKHR GetSurfaceHandle() const;

protected:
    void InitializeSurface() const;

private:
    vk::Instance mVulkanInstance;
    // 验证层
    UniquePtr<ValidationLayer> mValidationLayer;
    // 窗口表面
    UniquePtr<SurfaceBase> mSurface;

    // 动态加载各种函数用
    vk::DispatchLoaderDynamic mDynamicDispatcher;
};

RHI_VULKAN_NAMESPACE_END
