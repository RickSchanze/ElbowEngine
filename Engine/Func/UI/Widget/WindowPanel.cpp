//
// Created by Echo on 25-2-22.
//

#include "WindowPanel.h"

#include GEN_HEADER("Func.WindowPanel.generated.h")

GENERATED_SOURCE()

using namespace func;
using namespace ui;
using namespace widget;
using namespace core;


void WindowPanel::Draw(platform::rhi::CommandBuffer &cmd) {}

void WindowPanel::Rebuild(core::Rect2DI target_rect, core::Array<platform::rhi::Vertex_UI> &vertex_buffer,
                          core::Array<UInt32> &index_buffer) {}