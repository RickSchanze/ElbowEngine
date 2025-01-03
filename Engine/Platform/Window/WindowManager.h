/**
 * @file WindowManager.h
 * @author Echo 
 * @Date 24-11-22
 * @brief 
 */

#pragma once
#include "Core/Singleton/MManager.h"

namespace platform
{
class Window;
class WindowManager : public core::Manager<WindowManager>
{
public:
    [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::Top; }
    [[nodiscard]] core::StringView   GetName() const override { return "platform.WindowManager"; }

    void                  AddWindow(Window* window);
    bool                  RemoveWindow(int32_t window_id);
    bool                  RemoveWindow(Window* window);
    bool                  RemoveWindow(core::StringView window_title);
    [[nodiscard]] Window* GetWindow(int32_t window_id) const;
    [[nodiscard]] Window* GetWindow(core::StringView window_title) const;
    [[nodiscard]] Window* GetMainWindow() const { return GetWindow(0); }

protected:
    static inline int32_t next_window_id_ = 0;

    core::HashMap<int32_t, Window*> windows_;
};

inline WindowManager& GetWindowManager();

}   // namespace platform
