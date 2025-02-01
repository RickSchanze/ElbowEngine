//
// Created by Echo on 25-2-1.
//

#pragma once
#include "Core/Math/MathTypes.h"
#include "Core/Object/Object.h"
#include "Core/Object/ObjectPtr.h"

namespace core {
struct Vector2;
}
namespace platform::rhi {
class CommandBuffer;
}
namespace platform::rhi {
class Buffer;
}
namespace func::ui::widget {
class Widget;
}
namespace func::ui {

// 理想情况下 一个Overlay一个DrawCall
class CLASS() Overlay : public core::Object {
public:
  Overlay &SetSlot(widget::Widget *widget);

  void Draw(platform::rhi::CommandBuffer &cmd);

private:
  core::Vector3 position_{};
  core::Vector2 size_{};

  core::ObjectPtr<widget::Widget> slot_;
  core::SharedPtr<platform::rhi::Buffer> vertex_buffer_;
  core::SharedPtr<platform::rhi::Buffer> index_buffer_;
};

} // namespace func::ui
