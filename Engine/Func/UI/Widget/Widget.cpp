//
// Created by Echo on 25-2-1.
//

#include "Widget.h"

#include "Core/Reflection/CtorManager.h"
#include "Core/Reflection/Reflection.h"

#include GEN_HEADER("Func.Widget.generated.h")

GENERATED_SOURCE()

using namespace func;
using namespace ui;
using namespace widget;

void Widget::SetDirty(bool dirty) { dirty_ = dirty; }

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