//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Config/IConfig.hpp"
#include "Core/Core.hpp"
#include "Core/Math/Vector.hpp"
#include "Platform/RHI/Enums.hpp"
#include "Platform/Window/PlatformWindow.hpp"

class ECLASS(Config = "Config/Platform/PlatformConfig.cfg") PlatformConfig : public IConfig
{
    EFIELD()
    DEFINE_CFG_ITEM(bool, enable_multithread_render, EnableMultithreadRender, true)
    EFIELD()
    DEFINE_CFG_ITEM(bool, enable_validation_layer, EnableValidationLayer, true)
    EFIELD()
    DEFINE_CFG_ITEM(RHI::PresentMode, present_mode, PresentMode, RHI::PresentMode::VSync);

    EFIELD()
    DEFINE_CFG_ITEM(UInt32, swapchain_image_count, SwapchainImageCount, 2);

    EFIELD()
    DEFINE_CFG_ITEM(Vector2f, default_window_size, DefaultWindowSize, Vector2f(1920, 1080));

    EFIELD()
    DEFINE_CFG_ITEM(RHI::GraphicsAPI, api, GraphicsAPI, RHI::GraphicsAPI::Vulkan);

    EFIELD()
    DEFINE_CFG_ITEM(WindowLib, window_lib, WindowLib, WindowLib::GLFW);

    EFIELD()
    DEFINE_CFG_ITEM(Int32, window_flag, WindowFlag, 0);

    EFIELD()
    DEFINE_CFG_ITEM(UInt8, frame_count_in_flight, FrameCountInFlight, 2);

    #if USE_IMGUI
    EFIELD()
    DEFINE_CFG_ITEM(UInt32, min_imgui_image_sampler_pool_size, MinImGuiImageSamplerPoolSize, 100);

    EFIELD()
    DEFINE_CFG_ITEM(String, default_imgui_font_path, DefaultImGuiFontPath, "Assets/Font/Default.ttf");

    EFIELD()
    DEFINE_CFG_ITEM(UInt32, default_imgui_font_size, DefaultImGuiFontSize, 20);
    #endif

public:
    bool GetValidEnableValidationLayer() const;
    UInt8 GetValidFrameCountInFlight() const;
};
