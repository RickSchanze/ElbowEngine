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

#include GEN_HEADER("Platform.RHIConfig.generated.h")

namespace platform::rhi
{
class CLASS(Config = "Config/Platform/RHIConfig.cfg", Category = "Platform") RHIConfig : extends core::IConfig
{
    GENERATED_CLASS(RHIConfig)
    // 使用哪个API
    PROPERTY(Label = "使用图形API")
    DECLARE_CONFIG_ITEM(GraphicsAPI, api, API, GraphicsAPI::Vulkan);

    // 交换链图像数量
    PROPERTY(Label = "交换链图像数量")
    DECLARE_CONFIG_ITEM(int8_t, swapchain_image_count, SwapchainImageCount, 2)

    // 是否开启验证层
    PROPERTY(Label = "开启验证层", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "Vulkan")
    DECLARE_CONFIG_ITEM(bool, enable_validation_layer, EnableValidationLayer, true)

    PROPERTY(Label = "验证层名字", EnableWhen = "api == GraphicsAPI::Vulkan", Category = "Vulkan")
    DECLARE_CONFIG_ITEM(core::String, validation_layer_name, ValidationLayerName, {"VK_LAYER_KHRONOS_validation"})
};
} // namespace platform::rhi
