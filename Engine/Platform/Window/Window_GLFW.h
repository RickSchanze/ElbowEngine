/**
 * @file Window_GLFW.h
 * @author Echo 
 * @Date 24-11-22
 * @brief 
 */

#pragma once
#include "Window.h"
struct GLFWwindow;

namespace platform
{

class Window_GLFW final : public Window
{
public:
    Window_GLFW(core::StringView window_title, int width, int height, int flags);

    ~Window_GLFW() override;

    [[nodiscard]] void* GetNativeHandle() const override;

    void PollInputs() override;
    bool ShouldClose() override;
    void Close() override;

    rhi::Surface* CreateSurface(void* user_data, rhi::GraphicsAPI api) override;
    void          DestroySurface(core::Ref<rhi::Surface*> surface) override;

private:
    GLFWwindow* window_ = nullptr;
};

}   // namespace platform
