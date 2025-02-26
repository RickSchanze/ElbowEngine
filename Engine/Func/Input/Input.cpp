//
// Created by Echo on 25-2-26.
//

#include "Input.h"

#include "Core/Base/Ranges.h"
#include "Func/World/WorldClock.h"
#include "Platform/Window/WindowManager.h"

using namespace func;
using namespace platform;
using namespace core;

bool InputEventParam::IsKeyPressed(platform::KeyboardKey key) const { return range::Contains(pressed_key, key); }

bool Input::IsKeyPress(KeyboardKey key) {
  return WindowManager::GetWindow(0)->InternalGetInputStateRef().keyboard.keys_state[(Int32)key] ==
         KeyboardEvent::Press;
}

bool Input::IsKeyRelease(KeyboardKey key) {
  return WindowManager::GetWindow(0)->InternalGetInputStateRef().keyboard.keys_state[(Int32)key] ==
         KeyboardEvent::Release;
}

bool Input::IsMouseButtonDown(MouseButton key) {
  return WindowManager::GetWindow(0)->InternalGetInputStateRef().mouse.mouses_state[(Int32)key] == MouseEvent::Press;
}

bool Input::IsMouseButtonRelease(MouseButton key) {
  return WindowManager::GetWindow(0)->InternalGetInputStateRef().mouse.mouses_state[(Int32)key] == MouseEvent::Release;
}

Vector2 Input::GetMouseScroll() {
  auto &mouse = WindowManager::GetWindow(0)->InternalGetInputStateRef().mouse;
  return {(Float)mouse.scroll_dx, (Float)mouse.scroll_dy};
}

Vector2 Input::GetMousePos() {
  auto &mouse = WindowManager::GetWindow(0)->InternalGetInputStateRef().mouse;
  return {(Float)mouse.x, (Float)mouse.y};
}

void Input::DispatchInputEvent(const Millisecond &) {
  auto &self = GetByRef();
  InputEventParam event = self.CalculateInputEvent();
  self.FrameInputEvent.Invoke(event);
}

ManagerLevel Input::GetLevel() const { return ManagerLevel::L8; }

StringView Input::GetName() const { return "Input"; }

void Input::Startup() { TickEvents::PostInputTickEvent.AddBind(&Input::DispatchInputEvent); }

void Input::Shutdown() { FrameInputEvent.ClearBind(); }

InputEventParam Input::CalculateInputEvent() {
  InputEventParam rtn;
  // 键盘有两种响应: 按下和释放
  StaticArray<KeyboardKey, PROCESS_KEY_EVENT_COUNT> released_key_this_frame;
  range::Fill(released_key_this_frame, KeyboardKey::Count);
  StaticArray<KeyboardKey, PROCESS_KEY_EVENT_COUNT> pressed_key_this_frame;
  range::Fill(pressed_key_this_frame, KeyboardKey::Count);

  Int32 released_key_index = 0, pressed_key_index = 0;
  for (Int32 i = 0; i < (Int32)KeyboardKey::Count; i++) {
    KeyboardKey key_i = (KeyboardKey)i;
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

  Int32 mouse_button_count = GetEnumValue(MouseButton::Count);
  StaticArray<MouseButton, (Int32)MouseButton::Count> released_mouse_button_this_frame;
  range::Fill(released_mouse_button_this_frame, MouseButton::Count);
  StaticArray<MouseButton, (Int32)MouseButton::Count> pressed_mouse_button_this_frame;
  range::Fill(pressed_mouse_button_this_frame, MouseButton::Count);
  for (Int32 i = 0; i < mouse_button_count; i++) {
    MouseButton mouse_i = (MouseButton)i;
    if (Input::IsMouseButtonDown(mouse_i) &&
        previous_frame_state_.mouse.mouses_state[i] == MouseEvent::Release) { // 按下了
      pressed_mouse_button_this_frame[i] = mouse_i;
    }
    if (Input::IsMouseButtonRelease(mouse_i) &&
        previous_frame_state_.mouse.mouses_state[i] == MouseEvent::Press) { // 释放了
      released_mouse_button_this_frame[i] = mouse_i;
    }
  }

  Vector2 previous_mouse_pos = {(Float)previous_frame_state_.mouse.x, (Float)previous_frame_state_.mouse.y};
  Vector2 mouse_move = Input::GetMousePos() - previous_mouse_pos;
  rtn.mouse_move = mouse_move;
  rtn.mouse_scroll = Input::GetMouseScroll();
  rtn.pressed_key = pressed_key_this_frame;
  rtn.released_key = released_key_this_frame;
  rtn.pressed_mouse = pressed_mouse_button_this_frame;
  rtn.released_mouse = released_mouse_button_this_frame;
  previous_frame_state_ = WindowManager::GetWindow(0)->InternalGetInputStateRef();
  return rtn;
}