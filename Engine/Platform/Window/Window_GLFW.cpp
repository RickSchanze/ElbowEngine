/**
 * @file Window_GLFW.cpp
 * @author Echo
 * @Date 24-11-22
 * @brief
 */

#include "Window_GLFW.h"

#include "Core/Config/ConfigManager.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/PlatformDefines.h"
#include "Platform/PlatformLogcat.h"

#ifdef PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#define GLFW_INCLUDE_VULKAN
#include "Core/Base/Base.h"
#include "Core/Base/Ranges.h"
#include "Core/Profiler/ProfileMacro.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/Surface.h"
#include "WindowManager.h"
#include "vulkan/vulkan.h"

#include <range/v3/all.hpp>

using namespace platform;
using namespace core::range;
using namespace view;
using namespace action;

static bool glfw_callback_registered = false;

static void GLFWKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  // TODO: 这里应该是WindowID, 但是这里还没有实现
  Window *w = WindowManager::GetWindow(0);
  core::Assert::Require("Platform.Window.Input", w != nullptr, "Window not found");
  auto &input_state = w->InternalGetInputStateRef();
  KeyboardEvent cur_event;
  // clang-format off
  switch (action) {
    case GLFW_PRESS: cur_event = KeyboardEvent::Press; break;
    case GLFW_RELEASE: cur_event = KeyboardEvent::Release; break;
    default: cur_event = KeyboardEvent::Count;
  }
  switch (key) {
    case GLFW_KEY_A: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::A)] = cur_event; break;
    case GLFW_KEY_B: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::B)] = cur_event; break;
    case GLFW_KEY_C: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::C)] = cur_event; break;
    case GLFW_KEY_D: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::D)] = cur_event; break;
    case GLFW_KEY_E: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::E)] = cur_event; break;
    case GLFW_KEY_F: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::F)] = cur_event; break;
    case GLFW_KEY_G: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::G)] = cur_event; break;
    case GLFW_KEY_H: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::H)] = cur_event; break;
    case GLFW_KEY_I: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::I)] = cur_event; break;
    case GLFW_KEY_J: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::J)] = cur_event; break;
    case GLFW_KEY_K: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::K)] = cur_event; break;
    case GLFW_KEY_L: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::L)] = cur_event; break;
    case GLFW_KEY_M: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::M)] = cur_event; break;
    case GLFW_KEY_N: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::N)] = cur_event; break;
    case GLFW_KEY_O: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::O)] = cur_event; break;
    case GLFW_KEY_P: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::P)] = cur_event; break;
    case GLFW_KEY_Q: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::Q)] = cur_event; break;
    case GLFW_KEY_R: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::R)] = cur_event; break;
    case GLFW_KEY_S: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::S)] = cur_event; break;
    case GLFW_KEY_T: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::T)] = cur_event; break;
    case GLFW_KEY_U: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::U)] = cur_event; break;
    case GLFW_KEY_V: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::V)] = cur_event; break;
    case GLFW_KEY_W: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::W)] = cur_event; break;
    case GLFW_KEY_X: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::X)] = cur_event; break;
    case GLFW_KEY_Y: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::Y)] = cur_event; break;
    case GLFW_KEY_Z: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::Z)] = cur_event; break;
    case GLFW_KEY_SPACE: input_state.keyboard.keys_state[GetEnumValue(KeyboardKey::Space)] = cur_event; break;
    default:;
  }
  // clang-format on
}

static void GLFWMouseScrollCallback(GLFWwindow *window, Double x_offset, Double y_offset) {
  // TODO: 这里应该是WindowID, 但是这里还没有实现
  Window *w = WindowManager::GetWindow(0);
  core::Assert::Require("Platform.Window.Input", w != nullptr, "Window not found");
  auto &input_state = w->InternalGetInputStateRef();
  input_state.mouse.scroll_dx = x_offset;
  input_state.mouse.scroll_dy = y_offset;
}

static void GLFWMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  // TODO: 这里应该是WindowID, 但是这里还没有实现
  Window *w = WindowManager::GetWindow(0);
  core::Assert::Require("Platform.Window.Input", w != nullptr, "Window not found");
  auto &input_state = w->InternalGetInputStateRef();
  MouseEvent cur_event;
  // clang-format off
  switch (action) {
  case GLFW_PRESS: cur_event = MouseEvent::Press; break;
  case GLFW_RELEASE: cur_event = MouseEvent::Release; break;
  default: cur_event = MouseEvent::Count;
  }
  switch (button) {
  case GLFW_MOUSE_BUTTON_LEFT: input_state.mouse.mouses_state[GetEnumValue(MouseButton::Left)] = cur_event; break;
  case GLFW_MOUSE_BUTTON_RIGHT: input_state.mouse.mouses_state[GetEnumValue(MouseButton::Right)] = cur_event; break;
  case GLFW_MOUSE_BUTTON_MIDDLE: input_state.mouse.mouses_state[GetEnumValue(MouseButton::Middle)] = cur_event; break;
  default: NoEntry();
  }
  // clang-format on
}

static void GLFWMousePosCallback(GLFWwindow *window, double xpos, double ypos) {
  // TODO: 这里应该是WindowID, 但是这里还没有实现
  Window *w = WindowManager::GetWindow(0);
  core::Assert::Require("Platform.Window.Input", w != nullptr, "Window not found");
  auto &input_state = w->InternalGetInputStateRef();
  input_state.mouse.x = xpos;
  input_state.mouse.y = ypos;
}

static void GLFWErrorCallback(int error, const char *description) {
  LOGGER.Error(logcat::Platform_Window, "[GLFW] {}: {}", error, description);
}

static void GLFWWindowResizeCallback(GLFWwindow *window_glfw, Int32 width, Int32 height) {
  Window *window = GetWindowManager()._GetWindowByPtr(window_glfw);
  WindowEvents::OnWindowResize.Invoke(window, width, height);
  window->SetWidth(width);
  window->SetHeight(height);
}

static core::Array<core::String> GLFWGetVulkanExtensions(const core::Array<core::String> &extensions) {
  uint32_t count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
  core::Array<core::String> result;
  result.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    result[i] = glfw_extensions[i];
  }
  return Concat(result, extensions) | ToArray | Unique;
}

Window_GLFW::Window_GLFW(core::StringView title, int width, int height, int flags)
    : Window(title, width, height, flags) {
  core::Assert::Require(logcat::Platform_Window, glfwInit(), "Failed to initialize GLFW");
  auto config = core::GetConfig<PlatformConfig>();
  glfwSetErrorCallback(GLFWErrorCallback);
  if (GetFlags() & WF_NoWindowTitle) {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  }
  if (GetFlags() & WF_NoResize) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  }
  if (config->GetGraphicsAPI() == rhi::GraphicsAPI::Vulkan) {
    core::Assert::Require(logcat::Platform_Window, glfwVulkanSupported(), "GLFW: Vulkan not supported");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window_ = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().Data(), nullptr, nullptr);
  }
  if (!glfw_callback_registered) {
    if (config->GetGraphicsAPI() == rhi::GraphicsAPI::Vulkan) {
      rhi::Event_PostProcessVulkanExtensions.Bind(GLFWGetVulkanExtensions);
    }
    glfwSetFramebufferSizeCallback(window_, GLFWWindowResizeCallback);
    glfwSetKeyCallback(window_, GLFWKeyCallback);
    glfwSetScrollCallback(window_, GLFWMouseScrollCallback);
    glfwSetMouseButtonCallback(window_, GLFWMouseButtonCallback);
    glfwSetCursorPosCallback(window_, GLFWMousePosCallback);
    glfw_callback_registered = true;
  }
}

Window_GLFW::~Window_GLFW() {
  if (window_) {
    Close();
  }
}

void *Window_GLFW::GetNativeHandle() const { return window_; }

void Window_GLFW::PollInputs(const Millisecond &sec) {
  PROFILE_SCOPE_AUTO;
  glfwPollEvents();
}

bool Window_GLFW::ShouldClose() { return glfwWindowShouldClose(window_); }

void Window_GLFW::Close() {
  glfwDestroyWindow(window_);
  window_ = nullptr;
}

class GLFWSurface : public rhi::Surface {
public:
  explicit GLFWSurface(VkInstance instance, GLFWwindow *window) : instance_(instance) {
    auto result = glfwCreateWindowSurface(instance, window, nullptr, &surface_);
    core::Assert::Require(logcat::Platform_Window, result == VK_SUCCESS, "Failed to create window surface, code: {}",
                          static_cast<int32_t>(result));
  }

  ~GLFWSurface() override {
    if (surface_ && instance_) {
      vkDestroySurfaceKHR(instance_, surface_, nullptr);
    }
  }

  [[nodiscard]] void *GetNativeHandle() const override { return surface_; }

private:
  VkSurfaceKHR surface_ = nullptr;
  VkInstance instance_ = nullptr;
};

rhi::Surface *Window_GLFW::CreateSurface(void *user_data, rhi::GraphicsAPI api) {
  if (api == rhi::GraphicsAPI::Vulkan) {
    auto instance = static_cast<VkInstance>(user_data);
    return New<GLFWSurface>(instance, window_);
  }
  core::Assert::Require(logcat::Platform_Window, false, "Unsupported graphics API {} for create a surface",
                        GetEnumString(api));
  return nullptr;
}

void Window_GLFW::DestroySurface(core::Ref<rhi::Surface *> surface) {
  const auto surface_ptr = *surface;
  Delete(surface_ptr);
  surface = nullptr;
}
