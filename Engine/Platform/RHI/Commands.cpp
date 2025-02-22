//
// Created by Echo on 25-1-6.
//
#include "Commands.h"

#include "Image.h"
#include "ImageView.h"
#include "Platform/Window/Window.h"
#include "Platform/Window/WindowManager.h"

using namespace platform::rhi;
using namespace core;

Cmd_BeginRender::Cmd_BeginRender(const PooledArray<RenderAttachment> &colors_, const RenderAttachment &depth_,
                                 Size2D size_)
    : colors(colors_), depth(depth_) {
  if (size_.width <= 0 || size_.height <= 0) {
    render_size.width = GetWindowManager().GetMainWindow()->GetWidth();
    render_size.height = GetWindowManager().GetMainWindow()->GetHeight();
  } else {
    render_size = size_;
  }
}
