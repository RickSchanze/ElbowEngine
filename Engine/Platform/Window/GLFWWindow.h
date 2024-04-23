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
    explicit GlfwWindow(AnsiString InWindowTitle, const int InWidth, const int InHeight) :
        mWindowTitle(Move(InWindowTitle)), mWidth(InWidth), mHeight(InHeight) {}

    [[nodiscard]] bool IsValid() const { return mWindowHandle != nullptr; }

    UniquePtr<GLFWWindowSurface>     GetWindowSurface();
    [[nodiscard]] Array<const char*> GetRequiredExtensions() const;
    [[nodiscard]] GLFWwindow*        GetGLFWWindowHandle() const { return mWindowHandle; }

    void Initialize();
    void Finalize();
    void Tick();

private:
    GLFWwindow* mWindowHandle = nullptr;
    AnsiString  mWindowTitle;

    int mWidth;
    int mHeight;
};

PLATFORM_WINDOW_NAMESPACE_END
