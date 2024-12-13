/**
 * @file Window.cpp
 * @author Echo 
 * @Date 24-11-21
 * @brief 
 */

#include "Window.h"

#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Platform/Config/PlatformConfig.h"

#include "Core/Base/Base.h"
#include GEN_HEADER("Platform.Window.generated.h")
#include "Platform/PlatformLogcat.h"
#include "Window_GLFW.h"
#include "WindowManager.h"


GENERATED_SOURCE()

platform::Window::Window(core::StringView title, int32_t width, int32_t height, int32_t flags)
{
    auto* core_cfg     = core::GetConfig<core::CoreConfig>();
    auto* platform_cfg = core::GetConfig<PlatformConfig>();
    title_             = title.IsEmpty() ? core_cfg->GetAppName().ToStringView() : title;
    width_             = width == 0 ? platform_cfg->GetDefaultWindowSize().width : width;
    height_            = height == 0 ? platform_cfg->GetDefaultWindowSize().height : height;
    flags_             = flags == -1 ? platform_cfg->GetWindowFlag() : flags;
    WindowManager::Get()->AddWindow(this);
}

platform::Window* platform::CreateAWindow(WindowLib window_lib, core::StringView title, uint32_t width, uint32_t height, int32_t flags)
{
    if (window_lib == WindowLib::Count)
    {
        PlatformConfig* platform_cfg = core::GetConfig<PlatformConfig>();
        window_lib                   = platform_cfg->GetWindowLib();
    }
    switch (window_lib)
    {
    case WindowLib::GLFW: return New<Window_GLFW>(title, width, height, flags);
    default: core::Assert::Require(logcat::Platform_Window, false, "Window lib {} not supported.", GetEnumString(window_lib));
    }
    return nullptr;
}
