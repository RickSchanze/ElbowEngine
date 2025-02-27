//
// Created by Echo on 25-2-27.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Func/Input/Input.h"

namespace func::ui::widget {

class IInteractable {
public:
  virtual ~IInteractable() = default;

  virtual void OnMouseMove(Float x, Float y) {}
  // x,y 是以可交互物体左下角为原点鼠标的位置
  virtual void OnMousePressed(const RespondMouses &button, Int32 x, Int32 y) {}
  // x,y 是以可交互物体左下角为原点鼠标的位置
  virtual void OnMouseReleased(const RespondMouses &button, Int32 x, Int32 y) {}
  virtual void OnMouseScroll(Float x, Float y) {}

  virtual void OnKeyDown(const RespondKeys &key) {}
  virtual void OnKeyUp(const RespondKeys &key) {}
};

} // namespace func::ui::widget
