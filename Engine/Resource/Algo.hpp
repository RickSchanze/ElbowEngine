//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Math/Types.hpp"


namespace algo {
    class RectPacking {
    public:
        /**
         * 获得下一个可用的矩形范围
         * @param bound 整个矩形的大小
         * @param rects 已经被占用的矩形范围
         * @note 范围获取和rects顺序可能相关
         * @return
         */
        static Rect2Di GetNextAvailableRect(Vector2i bound, Span<Vector2i> rects);

        static Rect2Di GetNextAvailableRect(const Vector2i bound, const Span<Vector2i> rects, const Vector2i required_size);
    };
} // namespace algo
