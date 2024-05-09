/**
 * @file EngineApplication.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once
#include "../Platform/RHI/Vulkan/VulkanContext.h"
#include "CoreDef.h"
#include "RHI/Vulkan/Application.h"
#include "ToolCommon.h"
#include "Window/GLFWWindow.h"

namespace RHI::Vulkan {
class VulkanApplication;
}

TOOL_NAMESPACE_BEGIN

class EngineApplication {
public:
    // TODO: 自定义设定启动应用的名字、窗口大小、版本等
    // EngineApplication() = default;
    EngineApplication(const String& ProjectPath, const String& WindowTitle);

    ~EngineApplication() { Finitialize(); }

    static EngineApplication& Instance();

    void Initialize();
    void Finitialize() const;

    void Run();

    bool IsValid()const;

private:
    UniquePtr<RHI::Vulkan::VulkanApplication> mRenderApplication;
    UniquePtr<Platform::Window::GlfwWindow>   mWindow;

    static inline EngineApplication* mInstance = nullptr;

    String mWindowTitle;
};

TOOL_NAMESPACE_END
