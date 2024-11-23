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
#include "Core/Profiler/ProfileMacro.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/Surface.h"
#include "vulkan/vulkan.h"

#include <range/v3/all.hpp>

using namespace ranges::views;
using namespace ranges;

static void GLFWErrorCallback(int error, const char* description)
{
    LOGGER.Error(logcat::Platform_Window, "[GLFW] {}: {}", error, description);
}

static core::Array<core::String> GLFWGetVulkanExtensions(const core::Array<core::String>& extensions)
{
    uint32_t                  count           = 0;
    const char**              glfw_extensions = glfwGetRequiredInstanceExtensions(&count);
    core::Array<core::String> result;
    result.resize(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        result[i] = glfw_extensions[i];
    }
    return concat(result, extensions) | to_vector | actions::unique;
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
    if (config->GetGraphicsAPI() == GraphicsAPI::Vulkan)
    {
        Assert(logcat::Platform_Window, glfwVulkanSupported(), "GLFW: Vulkan not supported");
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window_ = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().Data(), nullptr, nullptr);
    }
    if (config->GetGraphicsAPI() == GraphicsAPI::Vulkan)
    {
        rhi::Event_PostProcessVulkanExtensions.Bind(GLFWGetVulkanExtensions);
    }
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

class GLFWSurface : public platform::rhi::Surface
{
public:
    explicit GLFWSurface(VkInstance instance, GLFWwindow* window) : instance_(instance)
    {
        auto result = glfwCreateWindowSurface(instance, window, nullptr, &surface_);
        Assert(logcat::Platform_Window, result == VK_SUCCESS, "Failed to create window surface, code: {}", static_cast<int32_t>(result));
    }

    ~GLFWSurface() override
    {
        if (surface_ && instance_)
        {
            vkDestroySurfaceKHR(instance_, surface_, nullptr);
        }
    }

    [[nodiscard]] void* GetNativeHandle() const override { return surface_; }

private:
    VkSurfaceKHR surface_  = nullptr;
    VkInstance   instance_ = nullptr;
};

platform::rhi::Surface* platform::Window_GLFW::CreateSurface(void* user_data, GraphicsAPI api)
{
    if (api == GraphicsAPI::Vulkan)
    {
        auto instance = static_cast<VkInstance>(user_data);
        return New<GLFWSurface>(instance, window_);
    }
    Assert(logcat::Platform_Window, false, "Unsupported graphics API {} for create a surface", GetEnumString(api));
    return nullptr;
}

void platform::Window_GLFW::DestroySurface(core::Ref<rhi::Surface*> surface)
{
    Delete(surface.GetPtr());
    surface = nullptr;
}
