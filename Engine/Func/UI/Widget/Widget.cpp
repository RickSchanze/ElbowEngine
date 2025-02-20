//
// Created by Echo on 25-2-1.
//

#include "Widget.h"

using namespace func;
using namespace ui;
using namespace widget;

void Widget::SetDirty(bool dirty) {
  dirty_ = dirty;
}

void Widget::SetVisible(bool visible) {
  if (visible != visible_) {
    visible_ = visible;
    SetDirty();
  }
}

void Widget::SetPadding(core::Vector4I padding) {
  if (padding != padding_) {
    padding_ = padding;
    SetDirty();
  }
}