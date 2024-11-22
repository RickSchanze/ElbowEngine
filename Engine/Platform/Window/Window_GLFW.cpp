/**
 * @file Window_GLFW.cpp
 * @author Echo 
 * @Date 24-11-22
 * @brief 
 */

#include "Window_GLFW.h"

#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/PlatformLogcat.h"
#include "Platform/PlatformDefines.h"

#ifdef PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include "Core/Profiler/ProfileMacro.h"
#include "GLFW/glfw3native.h"

#include <iostream>

void GLFWErrorCallback(int error, const char* description) {
    LOGGER.Error(logcat::Platform_Window, "[GLFW] {}: {}", error, description);
}

platform::Window_GLFW::Window_GLFW(core::StringView title, int width, int height, int flags) : Window(title, width, height, flags)
{
    Assert(logcat::Platform_Window, glfwInit(), "Failed to initialize GLFW");
    auto config = core::GetConfig<PlatformConfig>();
    glfwSetErrorCallback(GLFWErrorCallback);
    if (GetFlags() | WF_NoWindowTitle)
    {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }
    if (GetFlags() | WF_NoResize)
    {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    }
    if (config->GetGraphicsAPI() == rhi::GraphicsAPI::Vulkan)
    {
        Assert(logcat::Platform_Window, glfwVulkanSupported(), "GLFW: Vulkan not supported");
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window_ = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().Data(), nullptr, nullptr);
    }
    glfwMakeContextCurrent(window_);
}

platform::Window_GLFW::~Window_GLFW()
{
    if (window_)
    {
        Close();
    }
}

void* platform::Window_GLFW::GetNativeHandle() const
{
    return glfwGetWin32Window(window_);
}

void platform::Window_GLFW::PollInputs()
{
    PROFILE_SCOPE_AUTO;
    glfwPollEvents();
}

bool platform::Window_GLFW::ShouldClose()
{
    return glfwWindowShouldClose(window_);
}

void platform::Window_GLFW::Close()
{
    glfwDestroyWindow(window_);
    window_ = nullptr;
}
