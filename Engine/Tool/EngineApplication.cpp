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
#include "Window/GLFWWindow.h"

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
        mWindow->Tick(DeltaTime);
        mRenderApplication->Tick(DeltaTime);
        mWindow->EndImGuiFrame();
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

TOOL_NAMESPACE_END
