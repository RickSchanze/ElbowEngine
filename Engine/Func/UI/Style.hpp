//
// Created by Echo on 2025/3/29.
//

#pragma once

#include "Core/Math/Types.hpp"
#include "Core/Object/Object.hpp"

class Style : public Object {
    REFLECTED_CLASS(Style)
public:
    Color background_color = Color::FromUInt8(23, 23, 26);
};

REGISTER_TYPE(Style)
