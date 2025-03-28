//
// Created by Echo on 2025/3/25.
//
#include "Input.hpp"

#include "Func/World/WorldClock.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"

bool InputEventParam::IsKeyPressed(const KeyboardKey key) const { return pressed_keys.Contains(key); }

bool Input::IsKeyPress(KeyboardKey key) {
    return PlatformWindowManager::GetWindow(0)->InternalGetInputStateRef().keyboard.keys_state[static_cast<Int32>(key)] == KeyboardEvent::Press;
}

bool Input::IsKeyRelease(KeyboardKey key) {
    return PlatformWindowManager::GetWindow(0)->InternalGetInputStateRef().keyboard.keys_state[static_cast<Int32>(key)] == KeyboardEvent::Release;
}

bool Input::IsMouseButtonDown(MouseButton key) {
    return PlatformWindowManager::GetWindow(0)->InternalGetInputStateRef().mouse.mouses_state[static_cast<Int32>(key)] == MouseEvent::Press;
}

bool Input::IsMouseButtonRelease(MouseButton key) {
    return PlatformWindowManager::GetWindow(0)->InternalGetInputStateRef().mouse.mouses_state[static_cast<Int32>(key)] == MouseEvent::Release;
}

Vector2f Input::GetMouseScroll() {
    const auto &mouse = PlatformWindowManager::GetWindow(0)->InternalGetInputStateRef().mouse;
    return {static_cast<Float>(mouse.scroll_dx), static_cast<Float>(mouse.scroll_dy)};
}

Vector2f Input::GetMousePos() {
    const auto &mouse = PlatformWindowManager::GetWindow(0)->InternalGetInputStateRef().mouse;
    return {static_cast<Float>(mouse.x), static_cast<Float>(mouse.y)};
}

void Input::DispatchInputEvent(const MilliSeconds &) {
    auto &self = GetByRef();
    InputEventParam event = self.CalculateInputEvent();
    Evt_FrameInputEvent.Invoke(event);
}

bool Input::IsKeyPressed(const KeyboardKey key, RespondKeys pressed_keys) { return range::Contains(pressed_keys, key); }

bool Input::IsMouseButtonReleased(const MouseButton button, RespondMouses mouse_buttons) { return range::Contains(mouse_buttons, button); }
bool Input::IsMouseButtonPressed(const MouseButton button, RespondMouses mouse_buttons) { return range::Contains(mouse_buttons, button); }

bool Input::HasKeyRespond(RespondKeys pressed_keys) {
    return range::AnyOf(pressed_keys, [](const KeyboardKey &key) { return key != KeyboardKey::Count; });
}

bool Input::HasMouseButtonRespond(RespondMouses respond_mouses) {
    return range::AnyOf(respond_mouses, [](const MouseButton &key) { return key != MouseButton::Count; });
}

Float Input::GetLevel() const { return 12; }

StringView Input::GetName() const { return "Input"; }

void Input::Startup() { TickEvents::Evt_PostInputTick.AddBind(&Input::DispatchInputEvent); }

void Input::Shutdown() { Evt_FrameInputEvent.ClearBind(); }

InputEventParam Input::CalculateInputEvent() {
    InputEventParam rtn{};
    // 键盘有两种响应: 按下和释放
    StaticArray<KeyboardKey, PROCESS_KEY_EVENT_COUNT> released_key_this_frame;
    range::Fill(released_key_this_frame, KeyboardKey::Count);
    StaticArray<KeyboardKey, PROCESS_KEY_EVENT_COUNT> pressed_key_this_frame;
    range::Fill(pressed_key_this_frame, KeyboardKey::Count);

    Int32 released_key_index = 0, pressed_key_index = 0;
    for (Int32 i = 0; i < static_cast<Int32>(KeyboardKey::Count); i++) {
        const auto key_i = static_cast<KeyboardKey>(i);
        if (Input::IsKeyPress(key_i) && previous_frame_state_.keyboard.keys_state[i] == KeyboardEvent::Release) { // 按下了
            if (released_key_index < PROCESS_KEY_EVENT_COUNT) {
                pressed_key_this_frame[released_key_index++] = key_i;
            }
        }
        if (Input::IsKeyRelease(key_i) && previous_frame_state_.keyboard.keys_state[i] == KeyboardEvent::Press) { // 释放了
            if (pressed_key_index < PROCESS_KEY_EVENT_COUNT) {
                released_key_this_frame[pressed_key_index++] = key_i;
            }
        }
    }

    constexpr Int32 mouse_button_count = Int32(MouseButton::Count);
    StaticArray<MouseButton, static_cast<Int32>(MouseButton::Count)> released_mouse_button_this_frame;
    range::Fill(released_mouse_button_this_frame, MouseButton::Count);
    StaticArray<MouseButton, static_cast<Int32>(MouseButton::Count)> pressed_mouse_button_this_frame;
    range::Fill(pressed_mouse_button_this_frame, MouseButton::Count);
    for (Int32 i = 0; i < mouse_button_count; i++) {
        const auto mouse_i = static_cast<MouseButton>(i);
        if (Input::IsMouseButtonDown(mouse_i) && previous_frame_state_.mouse.mouses_state[i] == MouseEvent::Release) { // 按下了
            pressed_mouse_button_this_frame[i] = mouse_i;
        }
        if (Input::IsMouseButtonRelease(mouse_i) && previous_frame_state_.mouse.mouses_state[i] == MouseEvent::Press) { // 释放了
            released_mouse_button_this_frame[i] = mouse_i;
        }
    }

    const Vector2f previous_mouse_pos = {(Float) previous_frame_state_.mouse.x, (Float) previous_frame_state_.mouse.y};
    const Vector2f mouse_move = Input::GetMousePos() - previous_mouse_pos;
    rtn.mouse_move = mouse_move;
    rtn.mouse_scroll = Input::GetMouseScroll();
    rtn.pressed_keys = pressed_key_this_frame;
    rtn.released_keys = released_key_this_frame;
    rtn.pressed_mouse = pressed_mouse_button_this_frame;
    rtn.released_mouse = released_mouse_button_this_frame;
    rtn.mouse_pos = Input::GetMousePos();
    previous_frame_state_ = PlatformWindowManager::GetWindow(0)->InternalGetInputStateRef();
    return rtn;
}
