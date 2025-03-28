//
// Created by Echo on 2025/3/25.
//

#include "PlatformWindowManager.hpp"

#include "Core/Collection/Range/Range.hpp"
#include "Core/Logger/Logger.hpp"
#include "Core/Memory/New.hpp"
#include "PlatformWindow.hpp"

void PlatformWindowManager::AddWindow(PlatformWindow *window) {
    if (windows_.Empty()) {
        windows_[next_window_id_++] = window;
        return;
    }
    for (const auto my_window: windows_ | range::view::Values) {
        if (my_window->GetTitle() == window->GetTitle()) {
            Log(Error) << String::Format("Failed to add window, window with same title {} already exists. Destroy it.", *window->GetTitle());
            Delete(window);
            return;
        }
    }
    windows_[next_window_id_++] = window;
}

bool PlatformWindowManager::RemoveWindow(Int32 window_id) {
    if (windows_.Contains(window_id)) {
        windows_.Remove(window_id);
        return true;
    }
    return false;
}

bool PlatformWindowManager::RemoveWindow(StringView window_title) {
    return windows_.RemoveIf([window_title](const auto &pair) { return pair.second->GetTitle() == window_title; });
}

PlatformWindow *PlatformWindowManager::_GetWindowByPtr(const void *ptr) {
    for (const auto &window: windows_ | range::view::Values) {
        if (window->GetNativeHandle() == ptr) {
            return window;
        }
    }
    return nullptr;
}

PlatformWindow *PlatformWindowManager::InternalGetWindow(Int32 window_id) const {
    if (windows_.Contains(window_id)) {
        return windows_[window_id];
    }
    return nullptr;
}

PlatformWindow *PlatformWindowManager::InternalGetWindow(StringView window_title) const {
    for (const auto &window: windows_ | range::view::Values) {
        if (window->GetTitle() == window_title) {
            return window;
        }
    }
    return nullptr;
}
