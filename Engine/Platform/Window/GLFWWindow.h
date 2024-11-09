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

namespace rhi::vulkan
{
class RenderPass;
class IGraphicsPipeline;
class VulkanContext;
class ImguiGraphicsPipeline;
}   // namespace rhi::vulkan

namespace platform::window
{
struct GLFWWindowSurface : rhi::vulkan::SurfaceBase
{
    explicit GLFWWindowSurface(rhi::vulkan::Instance* InParentInstance, GLFWwindow* InWindow) : SurfaceBase(InParentInstance), mWindow(InWindow) {}

    void Initialize() override;

    GLFWwindow* mWindow;
};

class RealImGuiGraphicsPipeline;

class GlfwWindow
{
public:
    typedef GlfwWindow ThisClass;

    explicit GlfwWindow(core::String window_title, const int width, const int height) :
        window_title_(Move(window_title)), width_(width), height_(height)
    {
        g_engine_statistics.window_size.height = height;
        g_engine_statistics.window_size.width  = width;
    }

    [[nodiscard]] bool IsValid() const { return window_handle_ != nullptr; }

    core::UniquePtr<GLFWWindowSurface> GetWindowSurface();
    core::Array<const char*>           GetRequiredExtensions() const;
    GLFWwindow*                        GetGLFWWindowHandle() const { return window_handle_; }

    core::Size2D GetWindowSize();

    void SetFrameBufferResizedCallback(const GLFWframebuffersizefun Callback) const { glfwSetFramebufferSizeCallback(window_handle_, Callback); }

#ifdef USE_IMGUI
    void InitImGui(rhi::vulkan::VulkanContext* context);
    void SetupImGuiFonts();
    void ShutdownImGui() const;
    void BeginImGuiFrame();
    void EndImGuiFrame();
    void RegisterImGuiPipeline(rhi::vulkan::ImguiGraphicsPipeline** pipeline) const;
#endif

    void Initialize();
    void Finalize();

    void Tick(float DeltaTime);

    void SetMouseVisible(bool InVisible) const;




private:
    GLFWwindow* window_handle_ = nullptr;
    String      window_title_;

    RealImGuiGraphicsPipeline* imgui_graphics_pipeline_ = nullptr;

    int width_;
    int height_;
};

}   // namespace platform::window
