/**
 * @file GLFWWindow.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "GLFWWindow.h"
#include "Utils/StringUtils.h"
#include "vulkan/vk_enum_string_helper.h"

PLATFORM_WINDOW_NAMESPACE_BEGIN

void GLFWWindowSurface::Initialize() {
    if (mAttachedInstanceHandle->IsValid()) {
        VkSurfaceKHR   Surface{};
        const VkResult Result = glfwCreateWindowSurface(mAttachedInstanceHandle->GetVulkanInstanceHandle(), mWindow, nullptr, &Surface);
        if (Result != VK_SUCCESS) {
            String ErrorStr = StringUtils::FromAnsiString(string_VkResult(Result));
            throw VulkanException(std::format(L"创建窗口表面失败: {}", ErrorStr));
        }
        mSurfaceHandle = vk::SurfaceKHR(Surface);
    }
}

UniquePtr<GLFWWindowSurface> GlfwWindow::GetWindowSurface() {
    auto Surface = MakeUnique<GLFWWindowSurface>(nullptr, mWindowHandle);
    return Surface;
}

Array<const char*> GlfwWindow::GetRequiredExtensions() const {
    Array<const char*> Extensions;
    uint32_t           Count = 0;
    const char**       Names = glfwGetRequiredInstanceExtensions(&Count);
    for (uint32_t i = 0; i < Count; ++i) {
        Extensions.emplace_back(Names[i]);
    }
    return Extensions;
}

Size2D GlfwWindow::GetWindowSize() {
    glfwGetFramebufferSize(mWindowHandle, &mWidth, &mHeight);
    return Size2D(mWidth, mHeight);
}

void GlfwWindow::Initialize() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    const AnsiString Title = StringUtils::ToAnsiString(mWindowTitle);
    mWindowHandle          = glfwCreateWindow(mWidth, mHeight, Title.c_str(), nullptr, nullptr);
}

void GlfwWindow::Finalize() {
    glfwDestroyWindow(mWindowHandle);
    mWindowHandle = nullptr;
    glfwTerminate();
}

void GlfwWindow::Tick() {
    glfwPollEvents();
}

PLATFORM_WINDOW_NAMESPACE_END
