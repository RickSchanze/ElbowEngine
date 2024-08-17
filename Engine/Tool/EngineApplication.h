/**
 * @file EngineApplication.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "CoreEvents.h"
#include "Editor/Window/WindowBase.h"
#include "Math/MathTypes.h"
#include "RHI/Vulkan/Application.h"
#include "ToolCommon.h"
#include "Window/GLFWWindow.h"

class EditorImGuiStyle;

namespace Function
{
class GameObject;
class RenderContext;
}   // namespace Function
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

    ~EngineApplication();

    static EngineApplication& Instance();

    Size2D GetWindowSize() const;

    void LogBeginInit();

    void LogEndInit();

    void Initialize();
    void Finitialize() const;

    void Run();

    bool IsValid() const;

    // 此类自己的Tick 主要用来处理gEngineStatistics中的数据变化
    void InternalTick();

    static EngineApplication& Get() { return *instance_; }

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
            // 窗口下的"ImGui示例"
            void OnOpenImGuiDemoWindow();
            // 窗口下的"场景"
            void OnOpenSceneWindow();
    // clang-format on

private:
    static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        OnAppWindowResized.Broadcast(width, height);
    }

    TUniquePtr<RHI::Vulkan::VulkanApplication> render_application_;
    TUniquePtr<Platform::Window::GlfwWindow>   window_;
    Function::RenderContext*                   render_context_ = nullptr;

    static inline EngineApplication* instance_ = nullptr;

    String window_title_;

    // TODO: 封装Timer
    std::chrono::time_point<std::chrono::steady_clock> last_frame_time_;

    // 暂时测试
    Function::GameObject* camera_object_ = nullptr;

    EditorImGuiStyle* editor_style_;
};

TOOL_NAMESPACE_END
