/**
 * @file EngineApplication.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "EngineApplication.h"

#include "GLFW/glfw3.h"
#include "Path/Path.h"
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

EngineApplication::EngineApplication(const String& ProjectPath, const String& WindowTitle) {
    Path::SetProjectWorkPath(ProjectPath);
    if (mInstance != nullptr) {
        throw Exception(L"窗口实例已存在");
    }
    mWindowTitle = WindowTitle;
    mInstance    = this;
}

EngineApplication& EngineApplication::Instance() {
    if (!mInstance) {
        throw Exception(L"EngineApplication未初始化");
    }
    return *mInstance;
}

Size2D EngineApplication::GetWindowSize() const {
    return mWindow->GetWindowSize();
}

void EngineApplication::Initialize() {
    // 创建并初始化GlfwWindow
    mWindow = MakeUnique<Platform::Window::GlfwWindow>(mWindowTitle, 1920, 1080);
    mWindow->Initialize();
    mWindow->SetFrameBufferResizedCallback(&This::FrameBufferResizeCallback);
    // 创建并初始化VulkanApplication
    // 设置初始化RenderApplication需要的值
    mRenderApplication = MakeUnique<RHI::Vulkan::VulkanApplication>();
    auto Surface       = mWindow->GetWindowSurface();
    mRenderApplication->SetWindowSurface(Move(Surface));
    mRenderApplication->SetExtensions(mWindow->GetRequiredExtensions());
    mRenderApplication->Initialize();
    mWindow->InitImGui(mRenderApplication->GetContext());
}

void EngineApplication::Finitialize() const {
    if (!IsValid()) return;
    mWindow->ShutdownImGui();

    if (mRenderApplication->IsValid()) mRenderApplication->Finalize();
    if (mWindow->IsValid()) mWindow->Finalize();
}

void EngineApplication::Run() {
    static auto LastFrameTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(mWindow->GetGLFWWindowHandle())) {
        auto       CurrentFrameTime = std::chrono::high_resolution_clock::now();
        const auto DeltaTime        = std::chrono::duration<float>(CurrentFrameTime - LastFrameTime).count();
        LastFrameTime               = CurrentFrameTime;
        mWindow->BeginImGuiFrame();

        for (auto& SubWindow: mSubWindows) {
            SubWindow->Tick(DeltaTime);
        }

        DrawAppUI();

        ImGui::ShowDemoWindow();
        mWindow->Tick(DeltaTime);

        // 这个必须在最后一句 对ImGui的渲染在这里完成
        mRenderApplication->Tick(DeltaTime);
    }
}

bool EngineApplication::IsValid() const {
    if (!(mRenderApplication && mRenderApplication->IsValid())) return false;
    if (!(mWindow && mWindow->IsValid())) return false;
    return true;
}

void EngineApplication::SetMouseVisible(const bool InVisible) const {
    mWindow->SetMouseVisible(InVisible);
}

void EngineApplication::RemoveWindow(Window::WindowBase* InWindow) {
    ContainerUtils::Remove(mSubWindows, InWindow);
}

void EngineApplication::RemoveWindow(Type InType) {
    ContainerUtils::RemoveIf(mSubWindows, [InType](const Window::WindowBase* Window) {
        if (Window->GetType() == InType) {
            return true;
        }
        return false;
    });
}

// TODO: 更加可拓展的主窗口形式
void EngineApplication::DrawAppUI() {
    if (ImGui::BeginMainMenuBar()) {
        DrawWindowMenu();
        ImGui::EndMainMenuBar();
    }
}

void EngineApplication::DrawWindowMenu() {
    if (ImGui::BeginMenu(U8("窗口"))) {
        if (ImGui::MenuItem(U8("调试控制台"))) {
            OnOpenDebugWindow();
        }
        if (ImGui::MenuItem(U8("大纲"))) {
            OnOpenOutlineWindow();
        }
        if (ImGui::MenuItem(U8("细节"))) {
            OnOpenDetailWindow();
        }
        ImGui::EndMenu();
    }
}

template<typename T>
    requires std::is_base_of_v<Window::WindowBase, T>
void OpenWindow() {
    auto Window = WindowManager::GetWindow<T>();
    if (Window) {
        Window->SetVisible(Window::EWindowVisiable::Visiable);
    }
}

void EngineApplication::OnOpenDebugWindow() {
    OpenWindow<Window::DebugWindow>();
}

void EngineApplication::OnOpenOutlineWindow() {
    OpenWindow<Window::OutlineWindow>();
}

void EngineApplication::OnOpenDetailWindow(){
    OpenWindow<Window::DetailWindow>();
}

TOOL_NAMESPACE_END
