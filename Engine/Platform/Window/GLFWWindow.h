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
#include "Component/Camera.h"
#include "GLFW/glfw3.h"
#include "Math/MathTypes.h"
#include "RHI/Vulkan/Instance.h"
#include "RHI/Vulkan/Interface/IGraphicsPipeline.h"

namespace RHI::Vulkan {
class RenderPass;
}
namespace RHI::Vulkan {
class VulkanContext;
}
PLATFORM_WINDOW_NAMESPACE_BEGIN

struct GLFWWindowSurface : RHI::Vulkan::SurfaceBase
{
    explicit GLFWWindowSurface(RHI::Vulkan::Instance* InParentInstance, GLFWwindow* InWindow) :
        SurfaceBase(InParentInstance), mWindow(InWindow) {}

    void Initialize() override;

    GLFWwindow* mWindow;
};

class ImGuiGraphicsPipeline;

class GlfwWindow {
public:
    explicit GlfwWindow(String InWindowTitle, const int InWidth, const int InHeight) :
        mWindowTitle(Move(InWindowTitle)), mWidth(InWidth), mHeight(InHeight) {}

    [[nodiscard]] bool IsValid() const { return mWindowHandle != nullptr; }

    TUniquePtr<GLFWWindowSurface> GetWindowSurface();
    TArray<const char*>           GetRequiredExtensions() const;
    GLFWwindow*                   GetGLFWWindowHandle() const { return mWindowHandle; }

    Size2D GetWindowSize();

    void SetFrameBufferResizedCallback(const GLFWframebuffersizefun Callback) const {
        glfwSetFramebufferSizeCallback(mWindowHandle, Callback);
    }

    void InitImGui(Ref<RHI::Vulkan::VulkanContext> InContext);
    void SetupImGuiFonts();
    void ShutdownImGui();

    void BeginImGuiFrame();
    void EndImGuiFrame();

    void Initialize();
    void Finalize();

    void Tick(float DeltaTime);

    void SetMouseVisible(bool InVisible)const;


private:
    GLFWwindow* mWindowHandle = nullptr;
    String      mWindowTitle;

    Function::GameObject* mCameraObject;

    ImGuiGraphicsPipeline* mGraphicsPipeline;

    int mWidth;
    int mHeight;
};

PLATFORM_WINDOW_NAMESPACE_END
