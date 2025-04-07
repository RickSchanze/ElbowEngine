//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Config/IConfig.hpp"
#include "Core/Core.hpp"
#include "Core/Math/Types.hpp"
#include "Platform/RHI/Enums.hpp"
#include "Platform/Window/PlatformWindow.hpp"


class PlatformConfig : public IConfig {
    REFLECTED_CLASS(PlatformConfig)

    DEFINE_CFG_ITEM(bool, enable_multithread_render, EnableMultithreadRender, true)
    DEFINE_CFG_ITEM(bool, enable_validation_layer, EnableValidationLayer, true)
    DEFINE_CFG_ITEM(rhi::PresentMode, present_mode, PresentMode, rhi::PresentMode::VSync);
    DEFINE_CFG_ITEM(UInt32, swapchain_image_count, SwapchainImageCount, 2);
    DEFINE_CFG_ITEM(Vector2f, default_window_size, DefaultWindowSize, Vector2f(1920, 1080));
    DEFINE_CFG_ITEM(rhi::GraphicsAPI, api, GraphicsAPI, rhi::GraphicsAPI::Vulkan);
    DEFINE_CFG_ITEM(WindowLib, window_lib, WindowLib, WindowLib::GLFW);
    DEFINE_CFG_ITEM(Int32, window_flag, WindowFlag, 0);
    DEFINE_CFG_ITEM(UInt8, frame_count_in_flight, FrameCountInFlight, 2);
#if USE_IMGUI
    DEFINE_CFG_ITEM(UInt32, min_imgui_image_sampler_pool_size, MinImGuiImageSamplerPoolSize, 100);
    DEFINE_CFG_ITEM(String, default_imgui_font_path, DefaultImGuiFontPath, "Assets/Font/Default.ttf");
    DEFINE_CFG_ITEM(UInt32, default_imgui_font_size, DefaultImGuiFontSize, 20);
#endif

public:
    bool GetValidEnableValidationLayer() const;
    UInt8 GetValidFrameCountInFlight() const;
};
