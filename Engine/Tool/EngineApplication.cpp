/**
 * @file EngineApplication.cpp
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#include "EngineApplication.h"

#include "CoreGlobal.h"
#include "GLFW/glfw3.h"
#include "RHI/Vulkan/Application.h"
#include "Window/GLFWWindow.h"

TOOL_NAMESPACE_BEGIN

void EngineApplication::Initialize() {
    // 创建并初始化GlfwWindow
    mWindow = MakeUnique<Platform::Window::GlfwWindow>("Elbow Engine Editor", 1920, 1080);
    mWindow->Initialize();
    // 创建并初始化VulkanApplication
    mRenderApplication = MakeUnique<RHI::Vulkan::VulkanApplication>();
    // 设置初始化RenderApplication需要的值
    auto Surface       = mWindow->GetWindowSurface();
    Surface->SetInstanceHandle(&mRenderApplication->GetVulkanInstance());
    mRenderApplication->SetWindowSurface(Move(Surface));
    mRenderApplication->SetExtensions(mWindow->GetRequiredExtensions());
    mRenderApplication->Initialize();
}

void EngineApplication::Finitialize() const {
    if (mRenderApplication->IsValid()) mRenderApplication->Finalize();
    if (mWindow->IsValid()) mWindow->Finalize();
}

void EngineApplication::Run() {
    while (!glfwWindowShouldClose(mWindow->GetGLFWWindowHandle())) {
        mRenderApplication->Tick();
        mWindow->Tick();
    }
}

TOOL_NAMESPACE_END
