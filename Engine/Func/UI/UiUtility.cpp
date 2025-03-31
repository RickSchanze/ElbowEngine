//
// Created by Echo on 2025/3/29.
//

#include "UiUtility.hpp"

IMPL_REFLECTED(Padding) {
    return Type::Create<Padding>("Padding") | refl_helper::AddField("left", &Padding::left) | refl_helper::AddField("right", &Padding::right) |
           refl_helper::AddField("top", &Padding::top) | refl_helper::AddField("bottom", &Padding::bottom);
}

RectVertices UIUtility::ExtractVertexFromRect2D(Rect2Df rect) {
    RectVertices rtn;
    rtn.left_bottom.position = rect.pos;
    rtn.left_top.position = {rect.pos.x, rect.pos.y + rect.size.y, 0};
    rtn.right_bottom.position = {rect.pos.x + rect.size.x, rect.pos.y, 0};
    rtn.right_top.position = {rect.pos.x + rect.size.x, rect.pos.y + rect.size.y, 0};
    return rtn;
}

bool UIUtility::IsRectContainsPos(Rect2Df rect, Vector2f pos) {
    if (rect.size.x == NumberMax<float>() && rect.size.y == NumberMax<float>()) {
        return true;
    }
    if (rect.size.x == NumberMax<float>()) {
        return rect.pos.y <= pos.y && pos.y <= rect.pos.y + rect.size.y;
    }
    if (rect.size.y == NumberMax<float>()) {
        return rect.pos.x <= pos.x && pos.x <= rect.pos.x + rect.size.x;
    }
    return rect.pos.x <= pos.x && pos.x <= rect.pos.x + rect.size.x && rect.pos.y <= pos.y && pos.y <= rect.pos.y + rect.size.y;
}
