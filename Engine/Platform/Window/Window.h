/**
 * @file Window.h
 * @author Echo 
 * @Date 24-11-21
 * @brief 
 */

#pragma once
#include "Core/Core.h"
#include "Core/Utils/TimeUtils.h"
#include "Platform/RHI/Enums.h"
#include "Platform/RHI/GfxContext.h"

namespace platform::rhi
{
class Surface;
}
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
    explicit Window(core::StringView title = "", int32_t width = 0, int32_t height = 0, int32_t flags = -1);
    virtual ~Window() = default;

    [[nodiscard]] virtual void* GetNativeHandle() const = 0;

    virtual void PollInputs(const Millisecond& sec) = 0;
    virtual bool ShouldClose()                      = 0;
    virtual void Close()                            = 0;

    /**
     * 创建一个Surface
     * 这个函数主要用于GfxContext创建Surface, 满足Vulkan需求
     * @param instance 大部分时候是API数据, 例如如果是Vulkan可能传入的就是一个VkInstance, D3D12可能就是一个nullptr
     * @param api 表示当前在用什么API
     * @return
     */
    virtual rhi::Surface* CreateSurface(void* instance, rhi::GraphicsAPI api) = 0;

    /**
     * 销毁一个Surface
     * @param surface 需要销毁的Surface, 因为是一个Ref因此也会将传入的surface置为nullptr
     */
    virtual void DestroySurface(core::Ref<rhi::Surface*> surface) = 0;

    [[nodiscard]] core::StringView GetTitle() const { return title_; }
    [[nodiscard]] int32_t          GetWidth() const { return width_; }
    [[nodiscard]] int32_t          GetHeight() const { return height_; }
    [[nodiscard]] int32_t          GetFlags() const { return flags_; }

    /**
     * 下面俩函数用于设置窗口宽高
     * 被动设置, 也就是说当窗口大小变化时调用此函数调整width和height
     * 但是主动设置, 也就是当调用SetWidth和SetHeight时, 是未定义行为
     */
    void SetWidth(int32_t width) { width_ = width; }
    void SetHeight(int32_t height) { height_ = height; }

protected:
    core::StringView title_;
    int32_t          width_;
    int32_t          height_;
    int32_t          flags_;
};

DECLARE_MULTICAST_EVENT(WindowResizeEvent, Window*, Int32, Int32);

class WindowEvents
{
public:
    /**
     * 调用时, 传入的是新的数值, 可以通过GetWidth获取旧数值
     */
    static inline WindowResizeEvent OnWindowResize;
};

/**
 * 创建一个Window实例
 * @param window_lib window使用的窗口库
 * @param title 窗口标题 没有的话从PlatformConfig里取
 * @param width 窗口宽度 =0 则从PlatformConfig里取
 * @param height 窗口高度 =0 则从PlatformConfig里取
 * @param flags 窗口标志 =0 则从PlatformConfig里取
 * @return
 */
Window*
CreateAWindow(WindowLib window_lib = WindowLib::Count, core::StringView title = "", uint32_t width = 0, uint32_t height = 0, int32_t flags = 0);
}
