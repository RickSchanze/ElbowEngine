//
// Created by Echo on 25-2-22.
//

#include "Style.h"

using namespace func::ui;
using namespace core;

static Float scale = 1.f;

Color Style::Colors::PanelBackground() { return Color::FromUInt(23, 23, 26); }
Color Style::Colors::TitleBackground() { return Color::FromUInt(18, 18, 18); }
Color Style::Colors::FocusedTitleBackground() { return Color::FromUInt(60, 60, 60); }
Color Style::Colors::UIIcon() { return Color::FromUInt(255, 255, 255); }
Color Style::Colors::ButtonNormal() { return Color::FromUInt(74, 158, 255); }
Color Style::Colors::Text() { return Color::FromUInt(224, 224, 224); }

Float Style::GetGlobalScale() { return scale; }

void Style::SetGlobalScale(Float _scale) { scale = _scale; }