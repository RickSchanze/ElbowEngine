//
// Created by Echo on 2025/3/28.
//
#include "Commands.hpp"

#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"

using namespace rhi;

Cmd_BeginRender::Cmd_BeginRender(const Array<RenderAttachment> &colors_, const RenderAttachment &depth_,
                                 Vector2f size_)
    : colors(colors_), depth(depth_) {
    if (size_.x <= 0 || size_.y <= 0) {
        render_size.x = PlatformWindowManager::GetMainWindow()->GetWidth();
        render_size.y = PlatformWindowManager::GetMainWindow()->GetHeight();
    } else {
        render_size = size_;
    }
}