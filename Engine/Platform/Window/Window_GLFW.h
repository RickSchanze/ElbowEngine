/**
 * @file Window_GLFW.h
 * @author Echo 
 * @Date 24-11-22
 * @brief 
 */

#pragma once
#include "Window.h"
#include "GLFW/glfw3.h"

namespace platform {

class Window_GLFW final : public Window
{
public:
    Window_GLFW(core::StringView window_title, int width, int height, int flags);

    ~Window_GLFW() override;

    [[nodiscard]] void* GetNativeHandle() const override;

    void                PollInputs() override;
    bool                ShouldClose() override;
    void                Close() override;

private:
    GLFWwindow* window_= nullptr;
};

}
