//
// Created by Echo on 2025/3/30.
//

#pragma once
#include "Core/Math/Types.hpp"
#include "Core/TypeAlias.hpp"


class Window;
class UIEventDispatcher {
public:
    void ProcessInputEvent(const MilliSeconds &);

private:
    Int32 focused_window_handle_ = 0;
    Vector2f previous_mouse_pos_ = {};
};
