//
// Created by Echo on 25-2-22.
//

#include "VertexHelper.h"

#include "Core/Math/MathPipe.h"
#include "UIManager.h"
#include "UIMath.h"

using namespace func::ui;

void VertexHelper::AppendQuad(VertexWriteData &data, const platform::rhi::Vertex_UI &left_top,
                              const platform::rhi::Vertex_UI &left_bottom, const platform::rhi::Vertex_UI &right_top,
                              const platform::rhi::Vertex_UI &right_bottom) {
  Int32 width = UIManager::GetGlobalUIWidth(), height = UIManager::GetGlobalUIHeight();

  platform::rhi::Vertex_UI adjusted_left_top = left_top;
  adjusted_left_top.position =
      adjusted_left_top.position | core::Divide({width, height, 1}) | core::ToVector2 | UIPos2NDC;
  data.vertices.Add(adjusted_left_top);

  platform::rhi::Vertex_UI adjusted_left_bottom = left_bottom;
  adjusted_left_bottom.position =
      adjusted_left_bottom.position | core::Divide({width, height, 1}) | core::ToVector2 | UIPos2NDC;
  data.vertices.Add(adjusted_left_bottom);

  platform::rhi::Vertex_UI adjusted_right_top = right_top;
  adjusted_right_top.position =
      adjusted_right_top.position | core::Divide({width, height, 1}) | core::ToVector2 | UIPos2NDC;
  data.vertices.Add(adjusted_right_top);

  platform::rhi::Vertex_UI adjusted_right_bottom = right_bottom;
  adjusted_right_bottom.position =
      adjusted_right_bottom.position | core::Divide({width, height, 1}) | core::ToVector2 | UIPos2NDC;
  data.vertices.Add(adjusted_right_bottom);

  UInt64 index_end = data.vertex_offset + data.vertices.Size();
  data.indices.Add(index_end - 3);
  data.indices.Add(index_end - 1);
  data.indices.Add(index_end - 2);
  data.indices.Add(index_end - 3);
  data.indices.Add(index_end - 2);
  data.indices.Add(index_end - 4);
}

void VertexHelper::FillQuadUV(const core::Rect2D &uv_range, platform::rhi::Vertex_UI &left_top,
                              platform::rhi::Vertex_UI &left_bottom, platform::rhi::Vertex_UI &right_top,
                              platform::rhi::Vertex_UI &right_bottom) {
  left_top.uv.x = uv_range.position.x;
  left_top.uv.y = uv_range.position.y;

  left_bottom.uv.x = uv_range.position.x;
  left_bottom.uv.y = uv_range.position.y + uv_range.size.y;

  right_top.uv.x = uv_range.position.x + uv_range.size.x;
  right_top.uv.y = uv_range.position.y;

  right_bottom.uv.x = uv_range.position.x + uv_range.size.x;
  right_bottom.uv.y = uv_range.position.y + uv_range.size.y;
}

void VertexHelper::SetQuadColor(const core::Color &color, platform::rhi::Vertex_UI &left_top,
                                platform::rhi::Vertex_UI &left_bottom, platform::rhi::Vertex_UI &right_top,
                                platform::rhi::Vertex_UI &right_bottom) {
  left_top.color = color;
  left_bottom.color = color;
  right_top.color = color;
  right_bottom.color = color;
}

