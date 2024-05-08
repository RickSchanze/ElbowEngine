/**
 * @file EngineApplication.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "EngineApplication.h"

#include "../Platform/RHI/Vulkan/VulkanContext.h"
#include "GLFW/glfw3.h"
#include "Path/Path.h"
#include "RHI/Vulkan/Application.h"
#include "Window/GLFWWindow.h"

TOOL_NAMESPACE_BEGIN

EngineApplication::EngineApplication(const String& ProjectPath, const String& WindowTitle) {
    Path::SetProjectWorkPath(ProjectPath);
    mWindowTitle = WindowTitle;
    mInstance    = this;
}

EngineApplication& EngineApplication::Instance() {
    if (!mInstance) {
        throw Exception(L"EngineApplication未初始化");
    }
    return *mInstance;
}

void EngineApplication::Initialize() {
    // 创建并初始化GlfwWindow
    mWindow = MakeUnique<Platform::Window::GlfwWindow>(mWindowTitle, 1920, 1080);
    mWindow->Initialize();
    // 创建并初始化VulkanApplication
    // 设置初始化RenderApplication需要的值
    mRenderApplication = MakeUnique<RHI::Vulkan::VulkanApplication>();
    auto Surface = mWindow->GetWindowSurface();
    mRenderApplication->SetWindowSurface(Move(Surface));
    mRenderApplication->SetExtensions(mWindow->GetRequiredExtensions());
    mRenderApplication->Initialize();

    mMainContext = RHI::Vulkan::VulkanContext::CreateUnique(mRenderApplication->GetVulkanInstance());
}

void EngineApplication::Finitialize() const {
    if (!IsValid()) return;
    if (mMainContext->IsValid()) mMainContext->Finalize();
    if (mRenderApplication->IsValid()) mRenderApplication->Finalize();
    if (mWindow->IsValid()) mWindow->Finalize();
}

void EngineApplication::Run() {
    while (!glfwWindowShouldClose(mWindow->GetGLFWWindowHandle())) {
        mRenderApplication->Tick();
        mWindow->Tick();
    }
}

bool EngineApplication::IsValid() const {
    if (!(mRenderApplication && mRenderApplication->IsValid())) return false;
    if (!(mMainContext && mMainContext->IsValid())) return false;
    if (!(mWindow && mWindow->IsValid())) return false;
    return true;
}

TOOL_NAMESPACE_END
