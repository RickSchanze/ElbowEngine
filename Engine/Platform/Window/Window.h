/**
 * @file Window.h
 * @author Echo 
 * @Date 24-11-21
 * @brief 
 */

#pragma once
#include "Core/Core.h"


namespace platform
{
enum class ENUM() WindowLib
{
    GLFW,
    SDL3,
    Count,
};

enum ENUM(Flag) WindowFlag
{
    WF_NoWindowTitle = 1 << 0,   // 窗口没有标题栏
    WF_NoResize      = 1 << 1,   // 窗口不能调整大小
};

class Window
{
public:
    Window(core::StringView title = "", int32_t width = 0, int32_t height = 0, int32_t flags = -1);
    virtual ~Window() = default;

    [[nodiscard]] virtual void* GetNativeHandle() const = 0;

    virtual void PollInputs() = 0;
    virtual bool ShouldClose() = 0;
    virtual void Close()       = 0;

    [[nodiscard]] core::StringView GetTitle() const { return title_; }
    [[nodiscard]] int32_t          GetWidth() const { return width_; }
    [[nodiscard]] int32_t          GetHeight() const { return height_; }
    [[nodiscard]] int32_t          GetFlags() const { return flags_; }


protected:
    core::StringView title_;
    int32_t          width_;
    int32_t          height_;
    int32_t          flags_;
};

/**
 * 创建一个Window实例
 * @param window_lib window使用的窗口库
 * @param title 窗口标题 没有的话从PlatformConfig里取
 * @param width 窗口宽度 =0 则从PlatformConfig里取
 * @param height 窗口高度 =0 则从PlatformConfig里取
 * @param flags 窗口标志 =-1 则从PlatformConfig里取
 * @return
 */
Window*
CreateAWindow(WindowLib window_lib = WindowLib::Count, core::StringView title = "", uint32_t width = 0, uint32_t height = 0, int32_t flags = -1);
}
