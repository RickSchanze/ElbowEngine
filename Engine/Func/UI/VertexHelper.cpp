//
// Created by Echo on 25-2-22.
//

#include "VertexHelper.h"

using namespace func::ui;

void VertexHelper::AppendQuad(core::Array<platform::rhi::Vertex_UI> &vertex, core::Array<UInt32> &index,
                              const platform::rhi::Vertex_UI &left_top, const platform::rhi::Vertex_UI &left_bottom,
                              const platform::rhi::Vertex_UI &right_top, const platform::rhi::Vertex_UI &right_bottom) {
  vertex.push_back(left_top);
  vertex.push_back(left_bottom);
  vertex.push_back(right_top);
  vertex.push_back(right_bottom);

  UInt64 index_size = vertex.size();
  index.push_back(index_size - 3);
  index.push_back(index_size - 1);
  index.push_back(index_size - 2);
  index.push_back(index_size - 3);
  index.push_back(index_size - 2);
  index.push_back(index_size - 4);
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
