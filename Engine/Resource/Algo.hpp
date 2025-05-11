//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Math/Rect.hpp"

namespace NAlgo
{
class RectPacking
{
public:
    /**
     * 获得下一个可用的矩形范围
     * @param InBound 整个矩形的大小
     * @param InRects 已经被占用的矩形范围
     * @note 范围获取和rects顺序可能相关
     * @return
     */
    static Rect2Di GetNextAvailableRect(Vector2i InBound, Span<Vector2i> InRects);

    static Rect2Di GetNextAvailableRect(const Vector2i InBound, const Span<Vector2i> InRects, const Vector2i InRequiredSize);
};
} // namespace NAlgo
