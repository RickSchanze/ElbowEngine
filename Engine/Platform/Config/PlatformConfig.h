/**
 * @file PlatformConfig.h
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#pragma once
#include "Core/Config/IConfig.h"
#include "Core/Core.h"
#include "Core/Math/MathTypes.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/Window/Window.h"

#include GEN_HEADER("Platform.PlatformConfig.generated.h")

namespace platform
{
class CLASS(Config = "Config/Platform/PlatformConfig.cfg", Category = "Platform") PlatformConfig : extends core::IConfig
{
    GENERATED_CLASS(PlatformConfig)
    // 使用哪个API
    PROPERTY(Label = "使用图形API", Category = "Platform|RHI")
    DECLARE_CONFIG_ITEM(rhi::GraphicsAPI, api, GraphicsAPI, rhi::GraphicsAPI::Vulkan);

    // 窗口库
    PROPERTY(Label = "使用的窗口库", Category = "Platform")
    DECLARE_CONFIG_ITEM(WindowLib, window_lib, WindowLib, WindowLib::GLFW);

    PROPERTY(Label = "默认窗口大小", Category = "Platform")
    DECLARE_CONFIG_ITEM(core::Size2D, default_window_size, DefaultWindowSize, core::Size2D(1920, 1080));

    PROPERTY(Label = "默认窗口属性", Category = "Platform", EnumFlag = "platform.WindowFlag")
    DECLARE_CONFIG_ITEM(int, window_flag, WindowFlag, 0);

    PROPERTY(Label = "MSAA采样次数", Category = "Platform|RHI", EnumFlag = "platform.rhi.SampleCount")
    DECLARE_CONFIG_ITEM(int, msaa_sample_count, MSAASampleCount, 1);

    PROPERTY(Label = "呈现方式", Category = "Platform|RHI")
    DECLARE_CONFIG_ITEM(rhi::PresentMode, present_mode, PresentMode, rhi::PresentMode::VSync);

    // 交换链图像数量
    PROPERTY(Label = "交换链图像数量", Category = "Platform|RHI")
    DECLARE_CONFIG_ITEM(uint32_t, swapchain_image_count, SwapchainImageCount, 2);

    // 是否使用多线程渲染
    PROPERTY(Label = "是否使用多线程渲染", Category = "Platform|RHI")
    DECLARE_CONFIG_ITEM(bool, enable_multithread_render, EnableMultithreadRender, true);

    // 是否开启验证层
    PROPERTY(Label = "开启验证层", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "Platform|RHI|Vulkan")
    DECLARE_CONFIG_ITEM(bool, enable_validation_layer, EnableValidationLayer, true);

    // 提前渲染数量
    PROPERTY(Label = "开启验证层", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "Platform|RHI|Vulkan")
    DECLARE_CONFIG_ITEM(uint8_t, frame_count_in_flight, FrameCountInFlight, 2);

    // 验证层名字 一般不用修改
    PROPERTY(Label = "验证层名字", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "Platform|RHI|Vulkan")
    DECLARE_CONFIG_ITEM(core::String, validation_layer_name, ValidationLayerName, {"VK_LAYER_KHRONOS_validation"});

    PROPERTY(Label = "是否开启DynamicRendering特性", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "Platform|RHI|Vulkan")
    DECLARE_CONFIG_ITEM(bool, enable_dynamic_rendering, EnableDynamicRendering, true);

    PROPERTY(Label = "需要的扩展", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "Platform|RHI|Vulkan")
    DECLARE_CONFIG_ITEM(core::Array<core::String>, required_device_extensions, VulkanRequiredDeviceExtensions, {"VK_KHR_swapchain"});

public:
    uint8_t GetValidFrameCountInFlight() const;
};
} // namespace platform::rhi
