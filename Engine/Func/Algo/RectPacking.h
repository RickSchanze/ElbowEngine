//
// Created by Echo on 25-2-16.
//

#pragma once
#include "Core/Math/MathTypes.h"

#include <span>

namespace func::algo {
// 二叉树RectPacking算法实现
class RectPacking {
public:
  /**
   * 获得下一个可用的矩形范围
   * @param bound 整个矩形的大小
   * @param rects 已经被占用的矩形范围
   * @note 范围获取和rects顺序可能相关
   * @return
   */
  static core::Rect2DI GetNextAvailableRect(core::Vector2I bound, std::span<core::Vector2I> rects);

  static core::Rect2DI GetNextAvailableRect(core::Vector2I bound, std::span<core::Vector2I> rects, core::Vector2I required_size);
};

} // namespace func::algo
