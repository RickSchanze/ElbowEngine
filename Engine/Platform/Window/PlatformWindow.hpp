//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Collection/Range/Range.hpp"
#include "Core/Event/Event.hpp"
#include "Core/String/String.hpp"
#include "Core/TypeAlias.hpp"
#include "Platform/RHI/Enums.hpp"

#include GEN_HEADER("PlatformWindow.generated.hpp") // 不可删除且需要是最后一个

namespace RHI {
    class Surface;
}
enum class EENUM() WindowLib
{
    GLFW,
    SDL3,
    Count,
};

enum WindowFlag {
    WF_NoWindowTitle = 1 << 0, // 窗口没有标题栏
    WF_NoResize = 1 << 1, // 窗口不能调整大小
};

enum class MouseButton { Left, Right, Middle, Count };

enum class MouseEvent { Press, Release, Count };

struct MouseState {
    Float x, y;
    Float scroll_dx, scroll_dy;
    MouseEvent mouses_state[static_cast<Int32>(MouseButton::Count)];

    MouseState() : x(0), y(0), scroll_dx(0), scroll_dy(0) { range::Fill(mouses_state, MouseEvent::Release); }
};

// clang-format off
enum class KeyboardKey {
  A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
  Space,Enter,Escape,
  Count
};
// clang-format on

enum class KeyboardEvent { Press, Release, Count };

struct KeyboardState {
    KeyboardEvent keys_state[static_cast<Int32>(KeyboardKey::Count)];
    KeyboardState() { range::Fill(keys_state, KeyboardEvent::Release); }
};

// 底层的输入
struct LowLevelInputState {
    MouseState mouse;
    KeyboardState keyboard;

    bool IsKeyPressed(KeyboardKey key) const;
};

class PlatformWindow {

public:
    explicit PlatformWindow(StringView title = "", Int32 width = 0, Int32 height = 0, Int32 flags = -1);
    virtual ~PlatformWindow() = default;

    virtual void *GetNativeHandle() const = 0;

    virtual void PollInputs(const MilliSeconds &sec) = 0;
    virtual bool ShouldClose() = 0;
    virtual void Close() = 0;

    /**
     * 创建一个Surface
     * 这个函数主要用于GfxContext创建Surface, 满足Vulkan需求
     * @param instance 大部分时候是API数据, 例如如果是Vulkan可能传入的就是一个VkInstance, D3D12可能就是一个nullptr
     * @param api 表示当前在用什么API
     * @return
     */
    virtual RHI::Surface *CreateSurface(void *instance, RHI::GraphicsAPI api) = 0;

    /**
     * 销毁一个Surface
     * @param surface 需要销毁的Surface, 因为是一个Ref因此也会将传入的surface置为nullptr
     */
    virtual void DestroySurface(RHI::Surface *&surface) = 0;

    StringView GetTitle() const { return title_; }
    Int32 GetWidth() const { return width_; }
    Int32 GetHeight() const { return height_; }
    Int32 GetFlags() const { return flags_; }

    /**
     * 下面俩函数用于设置窗口宽高
     * 被动设置, 也就是说当窗口大小变化时调用此函数调整width和height
     * 但是主动设置, 也就是当调用SetWidth和SetHeight时, 是未定义行为
     */
    void SetWidth(Int32 width) { width_ = width; }
    void SetHeight(Int32 height) { height_ = height; }

    const LowLevelInputState &GetInputState() const { return input_state_; }

    // 用于实现Window的类里修改输入
    LowLevelInputState &InternalGetInputStateRef() { return input_state_; }

    virtual void BeginImGuiFrame() = 0;

protected:
    StringView title_;
    Int32 width_;
    Int32 height_;
    Int32 flags_;

    LowLevelInputState input_state_;
};

struct WindowResizeEvent : MulticastEvent<void, PlatformWindow *, Int32, Int32> {};

class WindowEvents {
public:
    /**
     * 调用时, 传入的是新的数值, 可以通过GetWidth获取旧数值
     */
    static inline WindowResizeEvent Evt_OnWindowResize;
};
