/**
 * @file EngineApplication.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "EngineApplication.h"

#include "GLFW/glfw3.h"
#include "Path/Path.h"
#include "Render/RenderContext.h"
#include "RHI/Vulkan/Application.h"

#include "Utils/ContainerUtils.h"

#include "UI/Window/DebugWindow.h"
#include "UI/Window/DetailWindow.h"
#include "UI/Window/OutlineWindow.h"
#include "UI/Window/WindowBase.h"
#include "UI/Window/WindowManager.h"

#include "Window/GLFWWindow.h"

#include <imgui.h>


TOOL_NAMESPACE_BEGIN

EngineApplication::EngineApplication(const String& project_path, const String& window_title)
{
    Path::SetProjectWorkPath(project_path);
    if (instance_ != nullptr)
    {
        throw Exception(L"窗口实例已存在");
    }
    window_title_ = window_title;
    instance_     = this;
}

EngineApplication& EngineApplication::Instance()
{
    if (!instance_)
    {
        throw Exception(L"EngineApplication未初始化");
    }
    return *instance_;
}

Size2D EngineApplication::GetWindowSize() const
{
    return window_->GetWindowSize();
}

void EngineApplication::Initialize()
{
    // 创建并初始化GlfwWindow
    window_ = MakeUnique<Platform::Window::GlfwWindow>(window_title_, 1920, 1080);
    window_->Initialize();
    window_->SetFrameBufferResizedCallback(&ThisClass::FrameBufferResizeCallback);
    // 创建并初始化VulkanApplication
    // 设置初始化RenderApplication需要的值
    render_application_ = MakeUnique<RHI::Vulkan::VulkanApplication>();
    auto Surface        = window_->GetWindowSurface();
    render_application_->SetWindowSurface(Move(Surface));
    render_application_->SetExtensions(window_->GetRequiredExtensions());
    render_application_->Initialize();
    window_->InitImGui(render_application_->GetContext());
    render_context_ = Function::RenderContext::Get();
}

void EngineApplication::Finitialize() const
{
    if (!IsValid()) return;
    window_->ShutdownImGui();

    if (render_application_->IsValid()) render_application_->Finalize();
    if (window_->IsValid()) window_->Finalize();
}

void EngineApplication::Run()
{
    while (!glfwWindowShouldClose(window_->GetGLFWWindowHandle()))
    {
        InternalTick();

        ASSERT_CATEGORY(Vulkan.Render, render_context_ != nullptr, "RenderContext未初始化");
        render_context_->PrepareFrameRender();

        window_->BeginImGuiFrame();

        for (auto& sub_window: sub_windows_)
        {
            sub_window->Tick(g_engine_statistics.time_delta);
        }

        DrawAppUI();

        ImGui::ShowDemoWindow();
        window_->Tick(g_engine_statistics.time_delta);

        render_context_->Draw();

        render_context_->PostFrameRender();
    }
}

bool EngineApplication::IsValid() const
{
    if (!(render_application_ && render_application_->IsValid())) return false;
    if (!(window_ && window_->IsValid())) return false;
    return true;
}

void EngineApplication::InternalTick()
{
    using namespace std::chrono;
    glfwSetInputMode(window_->GetGLFWWindowHandle(), GLFW_CURSOR, g_engine_statistics.is_hide_mouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    {
        auto current_frame_time        = steady_clock::now();
        g_engine_statistics.time_delta = duration<float>(current_frame_time - last_frame_time_).count();
        last_frame_time_               = current_frame_time;
        if (g_engine_statistics.time_delta > 0)   //
            g_engine_statistics.fps = static_cast<int32_t>(1.f / g_engine_statistics.time_delta);
    }
    g_engine_statistics.frame_count++;

    g_engine_statistics.object_count = ObjectManager::Get()->GetObjectCount();
}

void EngineApplication::RemoveWindow(Window::WindowBase* window)
{
    ContainerUtils::Remove(sub_windows_, window);
}

void EngineApplication::RemoveWindow(Type type)
{
    ContainerUtils::RemoveIf(sub_windows_, [type](const Window::WindowBase* window) {
        if (window->GetType() == type)
        {
            return true;
        }
        return false;
    });
}

// TODO: 更加可拓展的主窗口形式
void EngineApplication::DrawAppUI()
{
    if (ImGui::BeginMainMenuBar())
    {
        DrawWindowMenu();
        ImGui::EndMainMenuBar();
    }
}

void EngineApplication::DrawWindowMenu()
{
    if (ImGui::BeginMenu(U8("窗口")))
    {
        if (ImGui::MenuItem(U8("引擎数据统计")))
        {
            OnOpenStatisticsWindow();
        }
        if (ImGui::MenuItem(U8("大纲")))
        {
            OnOpenOutlineWindow();
        }
        if (ImGui::MenuItem(U8("细节")))
        {
            OnOpenDetailWindow();
        }
        ImGui::EndMenu();
    }
}

template<typename T>
    requires std::is_base_of_v<Window::WindowBase, T>
void OpenWindow()
{
    auto window = WindowManager::GetWindow<T>();
    if (window)
    {
        window->SetVisible(Window::EWindowVisiable::Visiable);
    }
}

void EngineApplication::OnOpenStatisticsWindow()
{
    OpenWindow<Window::StatisticsWindow>();
}

void EngineApplication::OnOpenOutlineWindow()
{
    OpenWindow<Window::OutlineWindow>();
}

void EngineApplication::OnOpenDetailWindow()
{
    OpenWindow<Window::DetailWindow>();
}

TOOL_NAMESPACE_END
