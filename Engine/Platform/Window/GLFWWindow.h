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

namespace RHI::Vulkan {
class RenderPass;
class IGraphicsPipeline;
class VulkanContext;
class ImguiGraphicsPipeline;
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
    typedef GlfwWindow ThisClass;
    explicit GlfwWindow(String window_title, const int width, const int height) :
        window_title_(Move(window_title)), width_(width), height_(height) {
        g_engine_statistics.window_size.height = height;
        g_engine_statistics.window_size.width  = width;
    }

    [[nodiscard]] bool IsValid() const { return window_handle_ != nullptr; }

    TUniquePtr<GLFWWindowSurface> GetWindowSurface();
    TArray<const char*>           GetRequiredExtensions() const;
    GLFWwindow*                   GetGLFWWindowHandle() const { return window_handle_; }

    Size2D GetWindowSize();

    void SetFrameBufferResizedCallback(const GLFWframebuffersizefun Callback) const {
        glfwSetFramebufferSizeCallback(window_handle_, Callback);
    }

    void InitImGui(Ref<RHI::Vulkan::VulkanContext> InContext);
    void SetupImGuiFonts();
    void ShutdownImGui();

    void BeginImGuiFrame();
    void EndImGuiFrame();

    void Initialize();
    void Finalize();

    void Tick(float DeltaTime);

    void SetMouseVisible(bool InVisible) const;

    void RegisterImGuiPipeline(RHI::Vulkan::ImguiGraphicsPipeline** pipeline);


private:
    GLFWwindow* window_handle_ = nullptr;
    String      window_title_;

    Function::GameObject* camera_object_ = nullptr;

    ImGuiGraphicsPipeline* imgui_graphics_pipeline_ = nullptr;

    int width_;
    int height_;
};

PLATFORM_WINDOW_NAMESPACE_END
