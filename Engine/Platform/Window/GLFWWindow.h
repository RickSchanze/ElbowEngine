/**
 * @file GLFWWindow.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "WindowCommon.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "Math/MathTypes.h"
#include "RHI/Vulkan/Instance.h"

PLATFORM_WINDOW_NAMESPACE_BEGIN

struct GLFWWindowSurface : RHI::Vulkan::SurfaceBase
{
    explicit GLFWWindowSurface(RHI::Vulkan::Instance* InParentInstance, GLFWwindow* InWindow) :
        SurfaceBase(InParentInstance), mWindow(InWindow) {}

    void Initialize() override;

    GLFWwindow* mWindow;
};

class GlfwWindow {
public:
    explicit GlfwWindow(String InWindowTitle, const int InWidth, const int InHeight) :
        mWindowTitle(Move(InWindowTitle)), mWidth(InWidth), mHeight(InHeight) {}

    [[nodiscard]] bool IsValid() const { return mWindowHandle != nullptr; }

    UniquePtr<GLFWWindowSurface>     GetWindowSurface();
    [[nodiscard]] Array<const char*> GetRequiredExtensions() const;
    [[nodiscard]] GLFWwindow*        GetGLFWWindowHandle() const { return mWindowHandle; }

    Size2D GetWindowSize();

    void SetFrameBufferResizedCallback(const GLFWframebuffersizefun Callback) const {
        glfwSetFramebufferSizeCallback(mWindowHandle, Callback);
    }

    void Initialize();
    void Finalize();
    void Tick();

private:
    GLFWwindow* mWindowHandle = nullptr;
    String      mWindowTitle;

    int mWidth;
    int mHeight;
};

PLATFORM_WINDOW_NAMESPACE_END
