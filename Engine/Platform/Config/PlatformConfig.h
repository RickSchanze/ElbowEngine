/**
 * @file RHIConfig.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Core/Config/IConfig.h"
#include "Core/Core.h"
#include "Platform/RHI/GfxContext.h"

#include "Core/Math/MathTypes.h"
#include GEN_HEADER("Platform.PlatformConfig.generated.h")
#include "Platform/Window/Window.h"

namespace platform
{
class CLASS(Config = "Config/Platform/PlatformConfig.cfg", Category = "Platform") PlatformConfig : extends core::IConfig
{
    GENERATED_CLASS(PlatformConfig)
    // 使用哪个API
    PROPERTY(Label = "使用图形API", Category = "RHI")
    DECLARE_CONFIG_ITEM(GraphicsAPI, api, GraphicsAPI, GraphicsAPI::Vulkan);

    // 窗口库
    PROPERTY(Label = "使用的窗口库", Category = "Platform")
    DECLARE_CONFIG_ITEM(WindowLib, window_lib, WindowLib, WindowLib::GLFW)

    PROPERTY(Label = "默认窗口大小", Category = "Platform")
    DECLARE_CONFIG_ITEM(core::Size2D, default_window_size, DefaultWindowSize, core::Size2D(1920, 1080))

    PROPERTY(Label = "默认窗口属性", Category = "Platform", EnumFlag = "platform.WindowFlag")
    DECLARE_CONFIG_ITEM(int, window_flag, WindowFlag, 0);

    PROPERTY(Label = "MSAA采样次数", Category = "Platform", EnumFlag = "platform.RHISampleCount")
    DECLARE_CONFIG_ITEM(int, msaa_sample_count, MSAASampleCount, 1)

    // 交换链图像数量
    PROPERTY(Label = "交换链图像数量", Category = "RHI")
    DECLARE_CONFIG_ITEM(int8_t, swapchain_image_count, SwapchainImageCount, 2)

    // 是否开启验证层
    PROPERTY(Label = "开启验证层", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "RHI|Vulkan")
    DECLARE_CONFIG_ITEM(bool, enable_validation_layer, EnableValidationLayer, true)

    // 验证层名字 一般不用修改
    PROPERTY(Label = "验证层名字", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "RHI|Vulkan")
    DECLARE_CONFIG_ITEM(core::String, validation_layer_name, ValidationLayerName, {"VK_LAYER_KHRONOS_validation"})

    PROPERTY(Label = "需要的扩展", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "RHI|Vulkan")
    DECLARE_CONFIG_ITEM(core::Array<core::String>, required_device_extensions, VulkanRequiredDeviceExtensions, {"VK_KHR_swapchain"})
};
} // namespace platform::rhi
