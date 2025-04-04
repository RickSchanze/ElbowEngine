//
// Created by Echo on 2025/3/29.
//

#include "Style.hpp"

IMPL_REFLECTED(Style) {
    return Type::Create<Style>("Style") | refl_helper::AddParents<Object>() |
           refl_helper::AddField("content_background_color", &ThisClass::content_background_color) |
           refl_helper::AddField("text_color", &ThisClass::text_color) |
           refl_helper::AddField("title_background_color", &ThisClass::title_background_color) |
           refl_helper::AddField("focused_title_background_color", &ThisClass::focused_title_background_color) |
           refl_helper::AddField("button_normal_color", &ThisClass::button_normal_color) |
           refl_helper::AddField("button_hover_color", &ThisClass::button_hover_color) |
           refl_helper::AddField("button_pressed_color", &ThisClass::button_pressed_color) |
           refl_helper::AddField("button_disabled_color", &ThisClass::button_disabled_color);
}
