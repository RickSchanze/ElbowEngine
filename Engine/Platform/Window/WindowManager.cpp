/**
 * @file WindowManager.cpp
 * @author Echo
 * @Date 24-11-22
 * @brief
 */

#include "WindowManager.h"

#include "Platform/PlatformLogcat.h"
#include "Window.h"
#include <range/v3/view/map.hpp>

using namespace ranges;
using namespace platform;

void WindowManager::AddWindow(Window *window) {
  if (windows_.empty()) {
    windows_[next_window_id_++] = window;
    return;
  }
  for (const auto my_window : windows_ | views::values) {
    if (my_window->GetTitle() == window->GetTitle()) {
      LOGGER.Error(logcat::Platform_Window,
                   "Failed to add window, window with same title {} already exists. Destroy it.", window->GetTitle());
      Delete(window);
      return;
    }
  }
  windows_[next_window_id_++] = window;
}

bool WindowManager::RemoveWindow(int32_t window_id) {
  if (windows_.contains(window_id)) {
    windows_.erase(window_id);
    return true;
  }
  return false;
}

bool WindowManager::RemoveWindow(Window *window) {
  for (auto it = windows_.begin(); it != windows_.end(); ++it) {
    if (it->second == window) {
      windows_.erase(it);
      return true;
    }
  }
  return false;
}

bool WindowManager::RemoveWindow(core::StringView window_title) {
  for (auto it = windows_.begin(); it != windows_.end(); ++it) {
    if (it->second->GetTitle() == window_title) {
      windows_.erase(it);
      return true;
    }
  }
  return false;
}

Window *WindowManager::InternalGetWindow(int32_t window_id) const {
  if (windows_.contains(window_id)) {
    return windows_.at(window_id);
  }
  return nullptr;
}

Window *WindowManager::InternalGetWindow(core::StringView window_title) const {
  for (const auto &window : windows_ | views::values) {
    if (window->GetTitle() == window_title) {
      return window;
    }
  }
  return nullptr;
}

Window *WindowManager::_GetWindowByPtr(void *ptr) {
  for (const auto &window : windows_ | views::values) {
    if (window->GetNativeHandle() == ptr) {
      return window;
    }
  }
  return nullptr;
}

Window *WindowManager::GetWindow(int32_t window_id) { return GetByRef().InternalGetWindow(window_id); }
Window *WindowManager::GetWindow(core::StringView window_title) { return GetByRef().InternalGetWindow(window_title); }
