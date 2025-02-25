/**
 * @file WindowManager.h
 * @author Echo
 * @Date 24-11-22
 * @brief
 */

#pragma once
#include "Core/Singleton/MManager.h"

namespace platform {
class Window;
class WindowManager : public core::Manager<WindowManager> {
public:
  [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L4; }
  [[nodiscard]] core::StringView GetName() const override { return "platform.WindowManager"; }

  void AddWindow(Window *window);
  bool RemoveWindow(int32_t window_id);
  bool RemoveWindow(Window *window);
  bool RemoveWindow(core::StringView window_title);


  Window *_GetWindowByPtr(void *ptr);

  [[nodiscard]] static Window *GetWindow(int32_t window_id);
  [[nodiscard]] static Window *GetWindow(core::StringView window_title);
  [[nodiscard]] static Window *GetMainWindow() { return GetWindow(0); }

protected:
  [[nodiscard]] Window *InternalGetWindow(int32_t window_id) const;
  [[nodiscard]] Window *InternalGetWindow(core::StringView window_title) const;
  [[nodiscard]] Window *InternalGetMainWindow() const { return InternalGetWindow(0); }

  static inline int32_t next_window_id_ = 0;

  core::HashMap<int32_t, Window *> windows_;
};

inline WindowManager &GetWindowManager() { return WindowManager::GetByRef(); }

} // namespace platform
