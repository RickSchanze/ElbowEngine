//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Config/IConfig.hpp"
#include "Core/Core.hpp"
#include "Core/Math/Vector.hpp"
#include "Platform/RHI/Enums.hpp"
#include "Platform/Window/PlatformWindow.hpp"

#include "Core/Math/Color.hpp"
#include GEN_HEADER("PlatformConfig.generated.hpp")

class ECLASS(Config = "Config/Platform/PlatformConfig.cfg") PlatformConfig : public IConfig
{
    GENERATED_BODY(PlatformConfig)

    EFIELD()
    DEFINE_CFG_ITEM(bool, mEnableMultithreadRender, EnableMultithreadRender, true)

    EFIELD()
    DEFINE_CFG_ITEM(bool, mEnableValidationLayer, EnableValidationLayer, true)

    EFIELD()
    DEFINE_CFG_ITEM(RHI::PresentMode, mPresentMode, PresentMode, RHI::PresentMode::VSync);

    EFIELD()
    DEFINE_CFG_ITEM(UInt32, mSwapchainImageCount, SwapchainImageCount, 2);

    EFIELD()
    DEFINE_CFG_ITEM(Vector2f, mDefaultWindowSize, DefaultWindowSize, Vector2f(1920, 1080));

    EFIELD()
    DEFINE_CFG_ITEM(RHI::GraphicsAPI, mGraphicsAPI, GraphicsAPI, RHI::GraphicsAPI::Vulkan);

    EFIELD()
    DEFINE_CFG_ITEM(WindowLib, mWindowLib, WindowLib, WindowLib::GLFW);

    EFIELD()
    DEFINE_CFG_ITEM(Int32, mWindowFlag, WindowFlag, 0);

    EFIELD()
    DEFINE_CFG_ITEM(UInt8, mFrameCountInFlight, FrameCountInFlight, 2);

    EFIELD()
    DEFINE_CFG_ITEM(UInt32, minImGuiImageSamplerPoolSize, MinImGuiImageSamplerPoolSize, 100);

    EFIELD()
    DEFINE_CFG_ITEM(String, mDefaultImGuiFontPath, DefaultImGuiFontPath, "Assets/Font/Default.ttf");

    EFIELD()
    DEFINE_CFG_ITEM(UInt32, mDefaultImGuiFontSize, DefaultImGuiFontSize, 20);

    EFIELD()
    DEFINE_CFG_ITEM(Color, mDefaultClearColor, DefaultClearColor, {0.3f, 0.3f, 0.3f, 1.0f});

public:
    bool GetValidEnableValidationLayer() const;
    UInt8 GetValidFrameCountInFlight() const;
};
