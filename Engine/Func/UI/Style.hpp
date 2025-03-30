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
    Float global_scale = 1.f;
};

REGISTER_TYPE(Style)
