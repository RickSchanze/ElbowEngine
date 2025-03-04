//
// Created by Echo on 25-2-22.
//

#pragma once
#include "Core/Math/MathTypes.h"

namespace func::ui::Style {
class Colors {
public:
  static core::Color PanelBackground();
  static core::Color TitleBackground();
  static core::Color FocusedTitleBackground();
  static core::Color UIIcon();
  static core::Color ButtonNormal();
  static core::Color Text();
};

Float GetGlobalScale();
void SetGlobalScale(Float scale);
} // namespace func::ui::Style

#define APPLY_SCALE(x) (x) * ::func::ui::Style::GetGlobalScale()
