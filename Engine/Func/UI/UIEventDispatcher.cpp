//
// Created by Echo on 2025/3/30.
//

#include "UIEventDispatcher.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Core/Profile.hpp"
#include "Func/Input/Input.hpp"
#include "UiManager.hpp"
#include "UiUtility.hpp"
#include "Widget/Window.hpp"

void UIEventDispatcher::ProcessInputEvent(const MilliSeconds &) {
    ProfileScope scope("UIEventDispatcher::ProcessInputEvent");
    auto pos = Input::GetMousePos();
    bool mouse_move = previous_mouse_pos_ != pos;
    Vector2f previous_mouse_pos = previous_mouse_pos_;
    previous_mouse_pos_ = pos;
    // 先处理鼠标按下事件
    {
        Window *focused_window = ObjectManager::GetObjectByHandle<Window>(focused_window_handle_);
        ProfileScope button_down_scope("MouseButtonDown");
        if (Input::IsMouseButtonPressed(MouseButton::Left)) {
            if (focused_window != nullptr) {
                if (UIUtility::IsRectContainsPos(focused_window->GetAbsoluteRect(), pos)) {
                    focused_window->OnMouseButtonPressed(MouseButton::Left, pos);
                } else {
                    focused_window->SetFocused(false);
                    focused_window_handle_ = 0;
                    for (auto &window: UIManager::GetWindows()) {
                        if (UIUtility::IsRectContainsPos(window->GetAbsoluteRect(), pos)) {
                            window->SetFocused(true);
                            window->OnMouseButtonPressed(MouseButton::Left, pos);
                            focused_window_handle_ = window->GetHandle();
                            break;
                        }
                    }
                }
            } else {
                for (auto &window: UIManager::GetWindows()) {
                    if (UIUtility::IsRectContainsPos(window->GetAbsoluteRect(), pos)) {
                        window->SetFocused(true);
                        window->OnMouseButtonPressed(MouseButton::Left, pos);
                        focused_window_handle_ = window->GetHandle();
                        break;
                    }
                }
            }
        }
    }
    // 处理鼠标抬起事件
    {
        Window *focused_window = ObjectManager::GetObjectByHandle<Window>(focused_window_handle_);
        if (Input::IsMouseButtonReleased(MouseButton::Left)) {
            if (focused_window != nullptr) {
                focused_window->OnMouseButtonReleased(MouseButton::Left, pos);
            }
        }
    }
    // 处理鼠标移动事件
    {
        // 重新获取一遍是因为鼠标按下事件可能会改变焦点窗口
        Window *focused_window = ObjectManager::GetObjectByHandle<Window>(focused_window_handle_);
        ProfileScope mouse_move_scope("MouseMove");
        if (mouse_move) {
            if (focused_window != nullptr) {
                focused_window->OnMouseMove(previous_mouse_pos, pos);
            }
        }
    }
}
