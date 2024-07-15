/**
 * @file EngineApplication.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Math/MathTypes.h"
#include "RHI/Vulkan/Application.h"
#include "ToolCommon.h"
#include "UI/Window/WindowBase.h"
#include "Window/GLFWWindow.h"

namespace Function
{
class GameObject;
class RenderContext;
}
namespace RHI::Vulkan
{
class VulkanApplication;
}

TOOL_NAMESPACE_BEGIN

class EngineApplication
{
public:
    typedef EngineApplication ThisClass;
    // TODO: 自定义设定启动应用的名字、窗口大小、版本等
    // EngineApplication() = default;

    EngineApplication(const String& project_path, const String& window_title);

    ~EngineApplication() { Finitialize(); }

    static EngineApplication& Instance();

    Size2D GetWindowSize() const;

    void Initialize();
    void Finitialize() const;

    void Run();

    bool IsValid() const;

    // 此类自己的Tick 主要用来处理gEngineStatistics中的数据变化
    void InternalTick();

    static EngineApplication& Get() { return *instance_; }

    bool frame_buffer_resized = false;

    void AddWindow(Window::WindowBase* window) { sub_windows_.push_back(window); }
    void RemoveWindow(Window::WindowBase* window);
    void RemoveWindow(Type type);

protected:
    // clang-format off
    // 为整个应用程序绘制UI 不抽象出独立Window
    void DrawAppUI();
    // 绘制主菜单上的"窗口"菜单
        void DrawWindowMenu();
            // 窗口下的"数据窗口"
            void OnOpenStatisticsWindow();
            // 窗口下的"大纲"
            void OnOpenOutlineWindow();
            // 窗口下的"细节"
            void OnOpenDetailWindow();
    // clang-format on

private:
    static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto& app                = Get();
        app.frame_buffer_resized = true;
    }

    TUniquePtr<RHI::Vulkan::VulkanApplication> render_application_;
    TUniquePtr<Platform::Window::GlfwWindow>   window_;
    Function::RenderContext*                   render_context_ = nullptr;

    static inline EngineApplication* instance_ = nullptr;

    String window_title_;

    TArray<Window::WindowBase*> sub_windows_;

    // TODO: 封装Timer
    std::chrono::time_point<std::chrono::steady_clock> last_frame_time_;

    // 暂时测试
    Function::GameObject* camera_object_ = nullptr;
};

TOOL_NAMESPACE_END
