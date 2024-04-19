/**
 * @file Core.h
 * @author Echo 
 * @Date 24-4-18
 * @brief Vulkan 核心
 */

#pragma once

#include <utility>

#include "CoreDef.h"
#include "vulkan/vulkan.hpp"

namespace Vulkan {

class Surface {
public:
    virtual ~Surface() = default;

    /**
     * 创建一个窗口表明对象
     * @param InInstance 窗口需要的Instance
     */
    virtual void Create(vk::Instance& InInstance) = 0;
    virtual void Finalize();

    explicit operator vk::SurfaceKHR() const { return mSurface; }
    explicit operator VkSurfaceKHR() const { return mSurface; }

    [[nodiscard]] vk::SurfaceKHR GetSurface() const { return mSurface; }

private:
    vk::SurfaceKHR mSurface{};
};

class ValidationLayer {
public:
    typedef ValidationLayer          ThisClass;
    static inline Array<const char*> sValidationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    ValidationLayer() = default;

    explicit ValidationLayer(const vk::Instance InAttachedInstance) :
        mAttachedInstance(InAttachedInstance) {}

#ifdef _DEBUG
    static constexpr bool sEnableValidation = true;
#else
    static constexpr bool sEnableValidation = false;
#endif

    /**
     * 检查是否支持验证层
     * @return
     */
    static bool CheckValidationLayerSupport();

    /**
     * 验证层消息回调函数
     * @param InMessageSeverity
     * @param InMessageType
     * @param InCallbackData
     * @param InUserData
     * @return
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      InMessageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             InMessageType,
        const VkDebugUtilsMessengerCallbackDataEXT* InCallbackData, void* InUserData
    );

    /**
     * 初始化此验证层
     */
    void Setup();

    /**
     * 设置此验证层附着到的实例
     * @param Instance 实例
     */
    void SetAttachedInstance(vk::Instance Instance);

    /**
     * 清理验证层
     */
    void Finalize();

    [[nodiscard]] vk::DebugUtilsMessengerEXT GetCallback() const { return mCallback; }
    [[nodiscard]] vk::Instance GetAttachedInstance() const { return mAttachedInstance; }

private:
    vk::DebugUtilsMessengerEXT mCallback{};
    // 附着的实例
    vk::Instance               mAttachedInstance{};
};

class Application {
public:
    typedef Application ThisClass;

    Application(
        const uint32_t InAppVersion, const uint32_t InEngineVersion,
        String InAppName = L"Elbow Engine", String InEngineName = L"ElbowEngine",
        const uint32 InApiVersion = VK_API_VERSION_1_3
    ) :
        mAppName(std::move(InAppName)), mAppVersion(InAppVersion),
        mEngineName(std::move(InEngineName)), mEngineVersion(InEngineVersion),
        mApiVersion(InApiVersion) {}

    /**
     * 析构函数 调用Finialize
     */
    ~Application();

    /**
     * 初始化Vulkan Application
     * @param InExtensions 需要启用的全局扩展列表
     * @param InExtensionCount 需要启用的扩展数量
     */
    void Init(const char** InExtensions, uint32 InExtensionCount);

    /**
     * 清理Vulkan Application
     */
    void Finalize();

    [[nodiscard]] vk::Instance     GetInstance() const { return mInstance; }
    [[nodiscard]] ValidationLayer& GetValidationLayer() { return mValidationLayer; }
    [[nodiscard]] bool             IsInitialized() const { return bInitialized; }
    [[nodiscard]] bool             IsFinalized() const { return bFinalized; }

protected:
    void CreateVulkanInstance(const char** InExtensions, uint32 InExtensionCount);

private:
    vk::Instance       mInstance{};
    ValidationLayer    mValidationLayer{};
    UniquePtr<Surface> mSurface;

    String mAppName;
    uint32 mAppVersion;
    String mEngineName;
    uint32 mEngineVersion;
    uint32 mApiVersion;

    // 是否已经初始化
    bool bInitialized = false;
    // 是否已经销毁
    bool bFinalized   = false;
};

}
