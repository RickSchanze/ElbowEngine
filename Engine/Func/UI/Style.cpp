//
// Created by Echo on 2025/3/29.
//

#include "Style.hpp"

IMPL_REFLECTED(Style) {
    return Type::Create<Style>("Style") | refl_helper::AddParents<Object>() |
           refl_helper::AddField("background_color", &ThisClass::background_color) | refl_helper::AddField("text_color", &ThisClass::text_color);
}
