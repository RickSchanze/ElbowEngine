//
// Created by Echo on 25-2-26.
//

#pragma once
#include "Platform/Config/PlatformConfig.h"

namespace func {

constexpr UInt64 PROCESS_KEY_EVENT_COUNT = 5;

struct InputEventParam {
  core::StaticArray<platform::KeyboardKey, PROCESS_KEY_EVENT_COUNT> released_key;
  core::StaticArray<platform::KeyboardKey, PROCESS_KEY_EVENT_COUNT> pressed_key;
  core::StaticArray<platform::MouseButton, (Int32)platform::MouseButton::Count> released_mouse;
  core::StaticArray<platform::MouseButton, (Int32)platform::MouseButton::Count> pressed_mouse;
  core::Vector2 mouse_move;
  core::Vector2 mouse_scroll;

  bool IsKeyPressed(platform::KeyboardKey key) const;
};

DECLARE_MULTICAST_EVENT(Event_FrameInput, const InputEventParam&);

class Input : public core::Manager<Input> {
public:
  static bool IsKeyPress(platform::KeyboardKey key);
  static bool IsKeyRelease(platform::KeyboardKey key);
  static bool IsMouseButtonDown(platform::MouseButton key);
  static bool IsMouseButtonRelease(platform::MouseButton key);
  static core::Vector2 GetMouseScroll();
  static core::Vector2 GetMousePos();
  static void DispatchInputEvent(const Millisecond&);

  core::ManagerLevel GetLevel() const override;
  core::StringView GetName() const override;
  void Startup() override;
  void Shutdown() override;

  static inline Event_FrameInput FrameInputEvent;

private:
  InputEventParam CalculateInputEvent();

  platform::LowLevelInputState previous_frame_state_;

};

} // namespace func
