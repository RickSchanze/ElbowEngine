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

namespace RHI::Vulkan {
class VulkanApplication;
}

TOOL_NAMESPACE_BEGIN

class EngineApplication {
public:
    typedef EngineApplication This;
    // TODO: 自定义设定启动应用的名字、窗口大小、版本等
    // EngineApplication() = default;
                              EngineApplication(const String& ProjectPath, const String& WindowTitle);

    ~EngineApplication() { Finitialize(); }

    static EngineApplication& Instance();

    Size2D GetWindowSize() const;

    void Initialize();
    void Finitialize() const;

    void Run();

    bool IsValid() const;

    // 此类自己的Tick 主要用来处理gEngineStatistics中的数据变化
    void InternalTick();

    static EngineApplication& Get() { return *mInstance; }

    bool bFrameBufferResized = false;

    void AddWindow(Window::WindowBase* InWindow) { mSubWindows.push_back(InWindow); }
    void RemoveWindow(Window::WindowBase* InWindow);
    void RemoveWindow(Type InType);

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
    static void FrameBufferResizeCallback(GLFWwindow* Window, int Width, int Height) {
        auto& App               = Get();
        App.bFrameBufferResized = true;
    }

    TUniquePtr<RHI::Vulkan::VulkanApplication> mRenderApplication;
    TUniquePtr<Platform::Window::GlfwWindow>   mWindow;

    static inline EngineApplication* mInstance = nullptr;

    String mWindowTitle;

    TArray<Window::WindowBase*> mSubWindows;

    // TODO: 封装Timer
    std::chrono::time_point<std::chrono::steady_clock> mLastFrameTime;
};

TOOL_NAMESPACE_END
