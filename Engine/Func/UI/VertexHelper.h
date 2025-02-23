//
// Created by Echo on 25-2-22.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Platform/RHI/VertexLayout.h"

namespace func::ui {

class VertexHelper {
public:
  static void AppendQuad(core::Array<platform::rhi::Vertex_UI> &vertex, core::Array<UInt32> &index,
                         const platform::rhi::Vertex_UI &left_top, const platform::rhi::Vertex_UI &left_bottom,
                         const platform::rhi::Vertex_UI &right_top, const platform::rhi::Vertex_UI &right_bottom);

  static void FillQuadUV(const core::Rect2D &uv_range, platform::rhi::Vertex_UI &left_top,
                         platform::rhi::Vertex_UI &left_bottom, platform::rhi::Vertex_UI &right_top,
                         platform::rhi::Vertex_UI &right_bottom);

  static void SetQuadColor(const core::Color &color, platform::rhi::Vertex_UI &left_top,
                           platform::rhi::Vertex_UI &left_bottom, platform::rhi::Vertex_UI &right_top,
                           platform::rhi::Vertex_UI &right_bottom);
};

} // namespace func::ui
