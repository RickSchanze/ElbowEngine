//
// Created by Echo on 2025/3/29.
//

#include "UiUtility.hpp"

RectVertices UIUtility::ExtractVertexFromRect2D(Rect2Df rect) {
    RectVertices rtn;
    rtn.left_bottom.position = rect.pos;
    rtn.left_top.position = {rect.pos.x, rect.pos.y + rect.size.y, 0};
    rtn.right_bottom.position = {rect.pos.x + rect.size.x, rect.pos.y, 0};
    rtn.right_top.position = {rect.pos.x + rect.size.x, rect.pos.y + rect.size.y, 0};
    return rtn;
}
