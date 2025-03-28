//
// Created by Echo on 2025/3/25.
//

#pragma once
#include "Core/Collection/StaticArray.hpp"
#include "Core/Math/Types.hpp"
#include "Core/TypeAlias.hpp"
#include "Platform/Window/PlatformWindow.hpp"

constexpr UInt64 PROCESS_KEY_EVENT_COUNT = 5;

typedef StaticArray<KeyboardKey, PROCESS_KEY_EVENT_COUNT> RespondKeys;
typedef StaticArray<MouseButton, static_cast<Int32>(MouseButton::Count)> RespondMouses;

struct InputEventParam {
    RespondKeys released_keys;
    RespondKeys pressed_keys;
    RespondMouses released_mouse;
    RespondMouses pressed_mouse;
    Vector2f mouse_move;
    Vector2f mouse_scroll;
    Vector2f mouse_pos;

    bool IsKeyPressed(KeyboardKey key) const;
};

struct FrameInputEvent : MulticastEvent<void, const InputEventParam &> {};

class Input : public Manager<Input> {
public:
    static bool IsKeyPress(KeyboardKey key);
    static bool IsKeyRelease(KeyboardKey key);
    static bool IsMouseButtonDown(MouseButton key);
    static bool IsMouseButtonRelease(MouseButton key);
    static Vector2f GetMouseScroll();
    static Vector2f GetMousePos();
    static void DispatchInputEvent(const MilliSeconds &);

    static bool IsKeyPressed(KeyboardKey key, RespondKeys pressed_keys);
    static bool IsMouseButtonReleased(MouseButton button, RespondMouses mouse_buttons);
    static bool IsMouseButtonPressed(MouseButton button, RespondMouses mouse_buttons);
    // 检查输入按键中有没有被按下的
    static bool HasKeyRespond(RespondKeys pressed_keys);
    static bool HasMouseButtonRespond(RespondMouses respond_mouses);

    [[nodiscard]] Float GetLevel() const override;
    [[nodiscard]] StringView GetName() const override;
    void Startup() override;
    void Shutdown() override;

    static inline FrameInputEvent Evt_FrameInputEvent;

private:
    InputEventParam CalculateInputEvent();

    LowLevelInputState previous_frame_state_;
};
