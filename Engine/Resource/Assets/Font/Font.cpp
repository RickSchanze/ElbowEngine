//
// Created by Echo on 25-1-30.
//

#include "Font.h"

#include "ft2build.h"
#include FT_FREETYPE_H

void resource::Font::PerformLoad() {}

void resource::Font::SetFontSize(Int16 new_size) {
  if (new_size != font_size_) {
    font_size_ = new_size;
    SetDirty(true);
  }
}