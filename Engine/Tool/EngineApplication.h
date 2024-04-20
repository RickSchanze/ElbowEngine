/**
 * @file EngineApplication.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "CoreDef.h"
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
    ~    EngineApplication() { Finitialize(); }
    void Initialize();
    void Finitialize() const;

    void Run();

private:
    SharedPtr<RHI::Vulkan::VulkanApplication> mRenderApplication;
    UniquePtr<Platform::Window::GlfwWindow>   mWindow;
};

TOOL_NAMESPACE_END
