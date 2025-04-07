//
// Created by Echo on 2025/4/7.
//

#include "Window.hpp"

IMPL_REFLECTED(Window) { return Type::Create<Window>("Window") | refl_helper::AddParents<Object>(); }

StringView Window::GetWindowTitle() { return "空白窗口"; }
