//
// Created by Echo on 2025/3/30.
//

#include "UIEventDispatcher.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Func/Input/Input.hpp"
#include "UiManager.hpp"
#include "UiUtility.hpp"
#include "Widget/Window.hpp"

static Vector2f AdjustPosition(Vector2f pos) {
    // glfw的坐标往下是负的, 例如左下角是0, -1080 右上角是1920, 0, 我们想转换到左下角0, 0 右下角1920, 1080
    return {};
}

void UIEventDispatcher::ProcessInputEvent(const MilliSeconds &) {
    auto pos = Input::GetMousePos();
    auto mouse_scroll = Input::GetMouseScroll();
    bool mouse_move = previous_mouse_pos_ != pos;
    Vector2f previous_mouse_pos = previous_mouse_pos_;
    previous_mouse_pos_ = pos;
    Window *focused_window = ObjectManager::GetObjectByHandle<Window>(focused_window_handle_);
    // 先处理点击事件
    if (focused_window != nullptr) {
        if (Input::IsMouseButtonRelease(MouseButton::Left)) {
            if (UIUtility::IsRectContainsPos(focused_window->GetUIRect(), pos)) {
                focused_window->OnMouseButtonUp(MouseButton::Left, pos);
                return;
            }
        }
        if (Input::IsMouseButtonDown(MouseButton::Left)) {
            if (UIUtility::IsRectContainsPos(focused_window->GetUIRect(), pos)) {
                focused_window->OnMouseButtonDown(MouseButton::Left, pos);
                return;
            } else {
                for (auto &window: UIManager::GetWindows()) {
                    if (UIUtility::IsRectContainsPos(window->GetUIRect(), pos)) {
                        window->SetFocused(true);
                        window->OnMouseButtonDown(MouseButton::Left, pos);
                        focused_window_handle_ = window->GetHandle();
                        focused_window->SetFocused(false);
                        return;
                    }
                }
                focused_window->SetFocused(false);
                focused_window_handle_ = 0;
            }
        }
        if (UIUtility::IsRectContainsPos(focused_window->GetUIRect(), pos)) {
            if (mouse_move) {
                focused_window->OnMouseMove(previous_mouse_pos, pos);
            }
            if (!mouse_scroll.IsZero()) {
                focused_window->OnMouseScroll(mouse_scroll);
            }
        }
    } else {
        if (Input::IsMouseButtonDown(MouseButton::Left)) {
            for (auto &window: UIManager::GetWindows()) {
                if (UIUtility::IsRectContainsPos(window->GetUIRect(), pos)) {
                    window->SetFocused(true);
                    window->OnMouseButtonDown(MouseButton::Left, pos);
                    focused_window_handle_ = window->GetHandle();
                    return;
                }
            }
        }
    }
}
