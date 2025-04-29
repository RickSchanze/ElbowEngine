//
// Created by Echo on 2025/3/25.
//

#pragma once
#include "Core/Collection/StaticArray.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/TypeAlias.hpp"
#include "Platform/Window/PlatformWindow.hpp"

constexpr UInt64 PROCESS_KEY_EVENT_COUNT = 5;

typedef StaticArray<KeyboardKey, PROCESS_KEY_EVENT_COUNT> RespondKeys;
typedef StaticArray<MouseButton, static_cast<Int32>(MouseButton::Count)> RespondMouses;

struct InputActions
{
    RespondKeys released_keys;
    RespondKeys pressed_keys;
    RespondMouses released_mouse;
    RespondMouses pressed_mouse;

    bool IsKeyPressed(KeyboardKey key) const;
};

class Input : public Manager<Input>
{
public:
    // 这里是状态
    static bool IsKeyPress(KeyboardKey key);
    static bool IsKeyRelease(KeyboardKey key);
    static bool IsMouseButtonDown(MouseButton key);
    static bool IsMouseButtonRelease(MouseButton key);
    static Vector2f GetMouseScroll();
    static Vector2f GetMousePos();

    // 这些是动作
    static bool IsMouseButtonReleased(MouseButton button);
    static bool IsMouseButtonPressed(MouseButton button);

    virtual Float GetLevel() const override;
    virtual StringView GetName() const override;
    virtual void Startup() override;
    virtual void Shutdown() override;

private:
    void CalculateInputEvent(const MilliSeconds&);

    LowLevelInputState previous_frame_state_;
    InputActions input_actions_ = {};
};
