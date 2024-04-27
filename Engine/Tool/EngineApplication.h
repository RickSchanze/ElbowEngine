/**
 * @file EngineApplication.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "RHI/Vulkan/Application.h"
#include "RHI/Vulkan/Render/VulkanRenderer.h"
#include "ToolCommon.h"
#include "Window/GLFWWindow.h"

namespace RHI::Vulkan {
class VulkanApplication;
}

TOOL_NAMESPACE_BEGIN

class EngineApplication {
public:
    // TODO: 自定义设定启动应用的名字、窗口大小、版本等
    EngineApplication() = default;
    EngineApplication(const String& ProjectPath, const String& WindowTitle);

    ~EngineApplication() { Finitialize(); }

    void Initialize();
    void Finitialize() const;

    void Run();

    [[nodiscard]] bool IsValid() const { return mRenderApplication && mRenderApplication->IsValid(); }

private:
    UniquePtr<RHI::Vulkan::VulkanApplication> mRenderApplication;
    SharedPtr<RHI::Vulkan::VulkanRenderer>    mRenderer;
    UniquePtr<Platform::Window::GlfwWindow>   mWindow;

    String mWindowTitle;
};

TOOL_NAMESPACE_END
