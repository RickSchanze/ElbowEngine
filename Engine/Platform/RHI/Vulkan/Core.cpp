/**
 * @file Core.cpp
 * @author Echo 
 * @Date 24-4-18
 * @brief 
 */

#include "Core.h"

#include "CoreGlobal.h"
#include "Exception.h"
#include "Utils/StringUtils.h"

void Vulkan::Surface::Finalize() {

}

bool Vulkan::ValidationLayer::CheckValidationLayerSupport() {
    // 获取所有可用的校验层列表
    auto AvailableLayers = vk::enumerateInstanceLayerProperties();
    // 检查是否所有ValidationLayers列表中的校验层都可以在AvailableLayer中找到
    for (const char* LayerName: sValidationLayers) {
        if (std::ranges::find_if(AvailableLayers, [LayerName](const auto& Elem) {
                return strcmp(LayerName, Elem.layerName) == 0;
            }) == AvailableLayers.end()) {
            return false;
        }
    }
    return true;
}
VkBool32 Vulkan::ValidationLayer::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      InMessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             InMessageType,
    const VkDebugUtilsMessengerCallbackDataEXT* InCallbackData, void* InUserData
) {
    switch (InMessageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        gLogger.Warning("[vulkan] {}", InCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        gLogger.Error("[vulkan] {}", InCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
    }
    // 如果是True vulkan api调用会返回VK_ERROR_VALIDATION_FAILED_EXT
    // 错误代码 通常只在测试校验层本身时设置为 True
    return VK_FALSE;
}

void Vulkan::ValidationLayer::Setup() {
    if constexpr (!sEnableValidation)
        return;
    else {
        vk::DebugUtilsMessengerCreateInfoEXT CreateInfo{};

        CreateInfo.setMessageSeverity(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        );
        CreateInfo.setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        );
        CreateInfo.setPfnUserCallback(&ThisClass::DebugCallback);

        const vk::DispatchLoaderDynamic Dispatcher{mAttachedInstance, vkGetInstanceProcAddr};

        auto Result = mAttachedInstance.createDebugUtilsMessengerEXT(
            &CreateInfo, nullptr, &mCallback, Dispatcher
        );

        if (Result != vk::Result::eSuccess) {
            throw VulkanException(L"校验层回调设置失败: 错误代码: " + std::to_wstring((int)Result));
        }
    }
}

void Vulkan::ValidationLayer::SetAttachedInstance(const vk::Instance Instance) {
    mAttachedInstance = Instance;
}

void Vulkan::ValidationLayer::Finalize() {
    if (sEnableValidation) {
        const vk::DispatchLoaderDynamic Dispatcher{mAttachedInstance, vkGetInstanceProcAddr};
        mAttachedInstance.destroyDebugUtilsMessengerEXT(mCallback, nullptr, Dispatcher);
    }
}

Vulkan::Application::~Application() {
    Finalize();
}

void Vulkan::Application::Init(const char** InExtensions, const uint32 InExtensionCount) {
    if (bInitialized) return;

    CreateVulkanInstance(InExtensions, InExtensionCount);
    mValidationLayer.Setup();

    bInitialized = true;
}

void Vulkan::Application::Finalize() {
    if (bFinalized) return;
    mValidationLayer.Finalize();
    mInstance.destroy(nullptr);
    bFinalized = true;
}

void Vulkan::Application::CreateVulkanInstance(
    const char** InExtensions, const uint32 InExtensionCount
) {
    if (ValidationLayer::sEnableValidation && !ValidationLayer::CheckValidationLayerSupport()) {
        throw VulkanException(L"请求使用验证层，但是找不到可用的验证层");
    }
    // 创建Vulkan实例
    const AnsiString          AppName    = StringUtils::ToAnsiString(mAppName);
    const AnsiString          EngineName = StringUtils::ToAnsiString(mEngineName);
    const vk::ApplicationInfo AppInfo(
        AppName.c_str(), mAppVersion, EngineName.c_str(), mEngineVersion, mApiVersion
    );
    vk::InstanceCreateInfo CreateInfo{};

    CreateInfo.setPApplicationInfo(&AppInfo)
        .setEnabledExtensionCount(InExtensionCount)
        .setPpEnabledExtensionNames(InExtensions);
    // 指定全局校验层
    if (ValidationLayer::sEnableValidation) {
        // 设置校验层
        CreateInfo.setEnabledLayerCount(ValidationLayer::sValidationLayers.size());
        CreateInfo.setPpEnabledLayerNames(ValidationLayer::sValidationLayers.data());
    } else {
        CreateInfo.ppEnabledLayerNames = nullptr;
    }
    Array<const char*> Extensions{InExtensions, InExtensions + InExtensionCount};
    if (ValidationLayer::sEnableValidation) {
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    CreateInfo.setEnabledExtensionCount(Extensions.size())
        .setPpEnabledExtensionNames(Extensions.data());
    mInstance = vk::createInstance(CreateInfo, nullptr);
    mValidationLayer.SetAttachedInstance(mInstance);
}
