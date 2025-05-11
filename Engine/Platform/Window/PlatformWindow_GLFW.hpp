//
// Created by Echo on 2025/3/23.
//


#pragma once
#include "PlatformWindow.hpp"

struct GLFWwindow;

class PlatformWindow_GLFW final : public PlatformWindow {
public:
    PlatformWindow_GLFW(StringView window_title, int width, int height, int flags);

    virtual ~PlatformWindow_GLFW() override;

    virtual void* GetNativeHandle() const override;

    virtual void PollInputs(const MilliSeconds& sec) override;
    virtual bool ShouldClose() override;
    virtual void Close() override;
    virtual void BeginImGuiFrame() override;

    virtual NRHI::Surface* CreateSurface(void* user_data, NRHI::GraphicsAPI api) override;
    virtual void DestroySurface(NRHI::Surface *&surface) override;

private:
    GLFWwindow *window_ = nullptr;
};
