//
// Created by Echo on 2025/3/25.
//
#include "PlatformWindow_GLFW.hpp"
#include "Core/Core.hpp"
#define GLFW_INCLUDE_VULKAN
#ifdef ELBOW_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include "Core/Config/ConfigManager.hpp"
#include "Core/Profile.hpp"
#include "GLFW/glfw3.h"
#include "Platform/Config/PlatformConfig.hpp"
#include "Platform/RHI/Events.hpp"
#include "Platform/RHI/Surface.hpp"
#include "PlatformWindowManager.hpp"
#include "imgui_impl_glfw.h"

static bool glfw_callback_registered = false;

static void GLFWKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // TODO: 这里应该是WindowID, 但是这里还没有实现
    PlatformWindow *w = PlatformWindowManager::GetWindow(0);
    Assert(w != nullptr, "Window not found");
    auto &input_state = w->InternalGetInputStateRef();
    KeyboardEvent cur_event;
    // clang-format off
  switch (action) {
    case GLFW_PRESS: cur_event = KeyboardEvent::Press; break;
    case GLFW_RELEASE: cur_event = KeyboardEvent::Release; break;
    default: cur_event = KeyboardEvent::Count;
  }
  switch (key) {
    case GLFW_KEY_A: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::A)] = cur_event; break;
    case GLFW_KEY_B: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::B)] = cur_event; break;
    case GLFW_KEY_C: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::C)] = cur_event; break;
    case GLFW_KEY_D: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::D)] = cur_event; break;
    case GLFW_KEY_E: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::E)] = cur_event; break;
    case GLFW_KEY_F: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::F)] = cur_event; break;
    case GLFW_KEY_G: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::G)] = cur_event; break;
    case GLFW_KEY_H: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::H)] = cur_event; break;
    case GLFW_KEY_I: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::I)] = cur_event; break;
    case GLFW_KEY_J: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::J)] = cur_event; break;
    case GLFW_KEY_K: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::K)] = cur_event; break;
    case GLFW_KEY_L: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::L)] = cur_event; break;
    case GLFW_KEY_M: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::M)] = cur_event; break;
    case GLFW_KEY_N: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::N)] = cur_event; break;
    case GLFW_KEY_O: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::O)] = cur_event; break;
    case GLFW_KEY_P: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::P)] = cur_event; break;
    case GLFW_KEY_Q: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::Q)] = cur_event; break;
    case GLFW_KEY_R: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::R)] = cur_event; break;
    case GLFW_KEY_S: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::S)] = cur_event; break;
    case GLFW_KEY_T: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::T)] = cur_event; break;
    case GLFW_KEY_U: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::U)] = cur_event; break;
    case GLFW_KEY_V: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::V)] = cur_event; break;
    case GLFW_KEY_W: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::W)] = cur_event; break;
    case GLFW_KEY_X: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::X)] = cur_event; break;
    case GLFW_KEY_Y: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::Y)] = cur_event; break;
    case GLFW_KEY_Z: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::Z)] = cur_event; break;
    case GLFW_KEY_SPACE: input_state.keyboard.keys_state[static_cast<Int32>(KeyboardKey::Space)] = cur_event; break;
    default:;
  }
    // clang-format on
}

static void GLFWMouseScrollCallback(GLFWwindow *window, Double x_offset, Double y_offset) {
    // TODO: 这里应该是WindowID, 但是这里还没有实现
    PlatformWindow *w = PlatformWindowManager::GetWindow(0);
    Assert(w != nullptr, "Window not found");
    auto &input_state = w->InternalGetInputStateRef();
    input_state.mouse.scroll_dx = x_offset;
    input_state.mouse.scroll_dy = y_offset;
}

static void GLFWMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    // TODO: 这里应该是WindowID, 但是这里还没有实现
    PlatformWindow *w = PlatformWindowManager::GetWindow(0);
    Assert(w != nullptr, "Window not found");
    auto &input_state = w->InternalGetInputStateRef();
    MouseEvent cur_event;
    // clang-format off
  switch (action) {
  case GLFW_PRESS: cur_event = MouseEvent::Press; break;
  case GLFW_RELEASE: cur_event = MouseEvent::Release; break;
  default: cur_event = MouseEvent::Count;
  }
  switch (button) {
  case GLFW_MOUSE_BUTTON_LEFT: input_state.mouse.mouses_state[static_cast<Int32>(MouseButton::Left)] = cur_event; break;
  case GLFW_MOUSE_BUTTON_RIGHT: input_state.mouse.mouses_state[static_cast<Int32>(MouseButton::Right)] = cur_event; break;
  case GLFW_MOUSE_BUTTON_MIDDLE: input_state.mouse.mouses_state[static_cast<Int32>(MouseButton::Middle)] = cur_event; break;
  default: Log(Fatal) << "无法处理的鼠标按键 "_es + String::FromInt(button);
  }
    // clang-format on
}

static void GLFWMousePosCallback(GLFWwindow *window, double xpos, double ypos) {
    // TODO: 这里应该是WindowID, 但是这里还没有实现
    PlatformWindow *w = PlatformWindowManager::GetWindow(0);
    Assert(w != nullptr, "Window not found");
    auto &input_state = w->InternalGetInputStateRef();
    input_state.mouse.x = xpos;
    // 修正为左下角为原点, 向上
    input_state.mouse.y = w->GetHeight() - ypos;
}

static void GLFWErrorCallback(int error, const char *description) { Log(Error) << String::Format("[GLFW] {}: {}", error, description); }

static void GLFWWindowResizeCallback(GLFWwindow *window_glfw, Int32 width, Int32 height) {
    PlatformWindow *window = GetWindowManager()._GetWindowByPtr(window_glfw);
    WindowEvents::Evt_OnWindowResize.Invoke(window, width, height);
    window->SetWidth(width);
    window->SetHeight(height);
}

static Array<String> GLFWGetVulkanExtensions(const Array<String> &extensions) {
    uint32_t count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
    Array<String> result;
    result.Resize(count);
    for (uint32_t i = 0; i < count; ++i) {
        result[i] = glfw_extensions[i];
    }
    Array<String> rtn;
    for (auto &ext: extensions) {
        if (!rtn.Contains(ext)) {
            rtn.Add(ext);
        }
    }
    for (auto &ext: result) {
        if (!rtn.Contains(ext)) {
            rtn.Add(ext);
        }
    }
    return rtn;
}

PlatformWindow_GLFW::PlatformWindow_GLFW(StringView window_title, int width, int height, int flags) :
    PlatformWindow(window_title, width, height, flags) {
    Assert(glfwInit(), "Failed to initialize GLFW");
    auto config = GetConfig<PlatformConfig>();
    glfwSetErrorCallback(GLFWErrorCallback);
    if (GetFlags() & WF_NoWindowTitle) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }
    if (GetFlags() & WF_NoResize) {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }
    if (config->GetGraphicsAPI() == rhi::GraphicsAPI::Vulkan) {
        Assert(glfwVulkanSupported(), "GLFW: Vulkan not supported");
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window_ = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().Data(), nullptr, nullptr);
    }
    if (!glfw_callback_registered) {
        if (config->GetGraphicsAPI() == rhi::GraphicsAPI::Vulkan) {
            rhi::Evt_PostProcessVulkanExtensions.Bind(GLFWGetVulkanExtensions);
        }
        glfwSetFramebufferSizeCallback(window_, GLFWWindowResizeCallback);
        glfwSetKeyCallback(window_, GLFWKeyCallback);
        glfwSetScrollCallback(window_, GLFWMouseScrollCallback);
        glfwSetMouseButtonCallback(window_, GLFWMouseButtonCallback);
        glfwSetCursorPosCallback(window_, GLFWMousePosCallback);
        glfw_callback_registered = true;
    }
#if USE_IMGUI
    ImGui_ImplGlfw_InitForVulkan(window_, true);
#endif
}

PlatformWindow_GLFW::~PlatformWindow_GLFW() {
    if (window_) {
        Close();
    }
}

void *PlatformWindow_GLFW::GetNativeHandle() const { return window_; }

void PlatformWindow_GLFW::PollInputs(const MilliSeconds &sec) {
    ProfileScope _(__func__);
    glfwPollEvents();
}

bool PlatformWindow_GLFW::ShouldClose() { return glfwWindowShouldClose(window_); }

void PlatformWindow_GLFW::Close() {
#if USE_IMGUI
    ImGui_ImplGlfw_Shutdown();
#endif
    glfwDestroyWindow(window_);
    window_ = nullptr;
}

void PlatformWindow_GLFW::BeginImGuiFrame() { ImGui_ImplGlfw_NewFrame(); }

class GLFWSurface : public rhi::Surface {
public:
    explicit GLFWSurface(VkInstance instance, GLFWwindow *window) : instance_(instance) {
        auto result = glfwCreateWindowSurface(instance, window, nullptr, &surface_);
        Assert(result == VK_SUCCESS, "Failed to create window surface, code: {}", static_cast<Int32>(result));
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

rhi::Surface *PlatformWindow_GLFW::CreateSurface(void *user_data, rhi::GraphicsAPI api) {
    if (api == rhi::GraphicsAPI::Vulkan) {
        auto instance = static_cast<VkInstance>(user_data);
        return New<GLFWSurface>(instance, window_);
    }
    Assert(false, "Unsupported graphics API {} for create a surface", GetEnumString(api));
    return nullptr;
}

void PlatformWindow_GLFW::DestroySurface(rhi::Surface *&surface) {
    Delete(surface);
    surface = nullptr;
}
