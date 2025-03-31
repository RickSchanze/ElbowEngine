//
// Created by Echo on 2025/3/29.
//

#pragma once

#include "Core/Math/Types.hpp"
#include "Core/Object/Object.hpp"

class Style : public Object {
    REFLECTED_CLASS(Style)
public:
    Color content_background_color = Color::FromUInt8(34, 34, 34);
    Color title_background_color = Color::FromUInt8(23, 23, 26);
    Color focused_title_background_color = Color::FromUInt8(50, 50, 50);
    Color text_color = Color::FromUInt8(233, 233, 233);
    Color button_normal_color = Color::FromUInt8(34, 102, 153);
    Color button_hover_color = Color::FromUInt8(23, 82, 133);
    Color button_pressed_color = Color::FromUInt8(12, 62, 113);
    // TODO: Disable状态 颜色写了但是还没实现
    Color button_disabled_color = Color::FromUInt8(170, 170, 170);
    Float global_scale = 1.f;
};

REGISTER_TYPE(Style)
