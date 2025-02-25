/**
 * @file Window.cpp
 * @author Echo
 * @Date 24-11-21
 * @brief
 */

#include "Window.h"

#include "Core/Base/Base.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "GLFW/glfw3.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/PlatformLogcat.h"
#include "WindowManager.h"
#include "Window_GLFW.h"

#include "Core/Base/Ranges.h"
#include GEN_HEADER("Platform.Window.generated.h")

GENERATED_SOURCE()

using namespace platform;
using namespace core;

MouseState::MouseState() : x(0), y(0), scroll_dx(0), scroll_dy(0) { range::Fill(mouses_state, MouseEvent::Release); }

KeyboardState::KeyboardState() { range::Fill(keys_state, KeyboardEvent::Release); }

Window::Window(StringView title, int32_t width, int32_t height, int32_t flags) {
  auto *core_cfg = GetConfig<CoreConfig>();
  auto *platform_cfg = GetConfig<PlatformConfig>();
  title_ = title.IsEmpty() ? core_cfg->GetAppName().ToStringView() : title;
  width_ = width == 0 ? platform_cfg->GetDefaultWindowSize().width : width;
  height_ = height == 0 ? platform_cfg->GetDefaultWindowSize().height : height;
  flags_ = flags == -1 ? platform_cfg->GetWindowFlag() : flags;
  WindowManager::Get()->AddWindow(this);
}

Window *platform::CreateAWindow(WindowLib window_lib, StringView title, uint32_t width, uint32_t height,
                                int32_t flags) {
  if (window_lib == WindowLib::Count) {
    PlatformConfig *platform_cfg = GetConfig<PlatformConfig>();
    window_lib = platform_cfg->GetWindowLib();
  }
  switch (window_lib) {
  case WindowLib::GLFW:
    return New<Window_GLFW>(title, width, height, flags);
  default:
    Assert::Require(logcat::Platform_Window, false, "Window lib {} not supported.", GetEnumString(window_lib));
  }
  return nullptr;
}
