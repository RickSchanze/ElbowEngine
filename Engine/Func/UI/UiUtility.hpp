//
// Created by Echo on 2025/3/29.
//

#pragma once
#include <tuple>

#include "Platform/RHI/VertexLayout.hpp"

struct RectVertices {
    rhi::Vertex_UI left_top;
    rhi::Vertex_UI right_top;
    rhi::Vertex_UI left_bottom;
    rhi::Vertex_UI right_bottom;
};

class UIUtility {
public:
    // 从一个矩形中提取它的四个顶点
    // 顺序
    static RectVertices ExtractVertexFromRect2D(Rect2Df rect);
};
