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
  static core::Color UIIconColor();
};

Float GetGlobalScale();
void SetGlobalScale(Float scale);
} // namespace func::ui::Style
