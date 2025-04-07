//
// Created by Echo on 2025/3/23.
//


#pragma once
#include "PlatformWindow.hpp"

struct GLFWwindow;

class PlatformWindow_GLFW final : public PlatformWindow {
public:
    PlatformWindow_GLFW(StringView window_title, int width, int height, int flags);

    ~PlatformWindow_GLFW() override;

    [[nodiscard]] void *GetNativeHandle() const override;

    void PollInputs(const MilliSeconds &sec) override;
    bool ShouldClose() override;
    void Close() override;
    void BeginImGuiFrame() override;

    rhi::Surface *CreateSurface(void *user_data, rhi::GraphicsAPI api) override;
    void DestroySurface(rhi::Surface *&surface) override;

private:
    GLFWwindow *window_ = nullptr;
};
