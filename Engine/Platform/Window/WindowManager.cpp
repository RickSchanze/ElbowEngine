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

void platform::WindowManager::AddWindow(Window* window)
{
    if (windows_.empty())
    {
        windows_[next_window_id_++] = window;
        return;
    }
    for (const auto my_window: windows_ | views::values)
    {
        if (my_window->GetTitle() == window->GetTitle())
        {
            LOGGER.Error(logcat::Platform_Window, "Failed to add window, window with same title {} already exists. Destroy it.", window->GetTitle());
            Delete(window);
            return;
        }
    }
    windows_[next_window_id_++] = window;
}

bool platform::WindowManager::RemoveWindow(int32_t window_id)
{
    if (windows_.contains(window_id))
    {
        windows_.erase(window_id);
        return true;
    }
    return false;
}

bool platform::WindowManager::RemoveWindow(Window* window)
{
    for (auto it = windows_.begin(); it != windows_.end(); ++it)
    {
        if (it->second == window)
        {
            windows_.erase(it);
            return true;
        }
    }
    return false;
}

bool platform::WindowManager::RemoveWindow(core::StringView window_title)
{
    for (auto it = windows_.begin(); it != windows_.end(); ++it)
    {
        if (it->second->GetTitle() == window_title)
        {
            windows_.erase(it);
            return true;
        }
    }
    return false;
}

platform::Window* platform::WindowManager::GetWindow(int32_t window_id) const
{
    if (windows_.contains(window_id))
    {
        return windows_.at(window_id);
    }
    return nullptr;
}

platform::Window* platform::WindowManager::GetWindow(core::StringView window_title) const
{
    for (const auto& window: windows_ | views::values)
    {
        if (window->GetTitle() == window_title)
        {
            return window;
        }
    }
    return nullptr;
}

platform::WindowManager& platform::GetWindowManager()
{
    return WindowManager::GetByRef();
}
