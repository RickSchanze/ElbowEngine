//
// Created by Echo on 2025/3/28.
//
#include "Commands.hpp"

#include "Core/Math/Vector.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"

using namespace RHI;

Cmd_BeginRender::Cmd_BeginRender(const Array<RenderAttachment> &colors_, const RenderAttachment &depth_,
                                 Vector2f size_)
    : colors(colors_), depth(depth_) {
    if (size_.X <= 0 || size_.Y <= 0) {
        render_size.X = PlatformWindowManager::GetMainWindow()->GetWidth();
        render_size.Y = PlatformWindowManager::GetMainWindow()->GetHeight();
    } else {
        render_size = size_;
    }
}