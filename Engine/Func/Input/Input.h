//
// Created by Echo on 25-2-26.
//

#pragma once
#include "Platform/Config/PlatformConfig.h"

namespace func {

constexpr UInt64 PROCESS_KEY_EVENT_COUNT = 5;

typedef core::StaticArray<platform::KeyboardKey, PROCESS_KEY_EVENT_COUNT> RespondKeys;
typedef core::StaticArray<platform::MouseButton, (Int32)platform::MouseButton::Count> RespondMouses;

struct InputEventParam {
  RespondKeys released_keys;
  RespondKeys pressed_keys;
  RespondMouses released_mouse;
  RespondMouses pressed_mouse;
  core::Vector2 mouse_move;
  core::Vector2 mouse_scroll;
  core::Vector2 mouse_pos;

  bool IsKeyPressed(platform::KeyboardKey key) const;
};

DECLARE_MULTICAST_EVENT(Event_FrameInput, const InputEventParam &);

class Input : public core::Manager<Input> {
public:
  static bool IsKeyPress(platform::KeyboardKey key);
  static bool IsKeyRelease(platform::KeyboardKey key);
  static bool IsMouseButtonDown(platform::MouseButton key);
  static bool IsMouseButtonRelease(platform::MouseButton key);
  static core::Vector2 GetMouseScroll();
  static core::Vector2 GetMousePos();
  static void DispatchInputEvent(const Millisecond &);

  static bool IsKeyPressed(platform::KeyboardKey key, RespondKeys pressed_keys);
  static bool IsMouseButtonReleased(platform::MouseButton button, RespondMouses mouse_buttons);
  static bool IsMouseButtonPressed(platform::MouseButton button, RespondMouses mouse_buttons);
  // 检查输入按键中有没有被按下的
  static bool HasKeyRespond(RespondKeys pressed_keys);
  static bool HasMouseButtonRespond(RespondMouses respond_mouses);

  [[nodiscard]] core::ManagerLevel GetLevel() const override;
  [[nodiscard]] core::StringView GetName() const override;
  void Startup() override;
  void Shutdown() override;

  static inline Event_FrameInput FrameInputEvent;

private:
  InputEventParam CalculateInputEvent();

  platform::LowLevelInputState previous_frame_state_;
};

} // namespace func
