//
// Created by Echo on 2025/3/23.
//

#include "Window.hpp"
#include "Core/Config/ConfigManager.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "PlatformWindow.hpp"
#include "PlatformWindow_GLFW.hpp"

PlatformWindow *CreatePlatformWindow(WindowLib window_lib, StringView title, UInt32 width, UInt32 height, Int32 flags) {
    if (window_lib == WindowLib::Count) {
        PlatformConfig *platform_cfg = GetConfig<PlatformConfig>();
        window_lib = platform_cfg->GetWindowLib();
    }
    switch (window_lib) {
        case WindowLib::GLFW:
            return New<PlatformWindow_GLFW>(title, width, height, flags);
        default:
            Log(Fatal) << String::Format("Window lib {} not supported", static_cast<Int32>(window_lib));
    }
    return nullptr;
}
