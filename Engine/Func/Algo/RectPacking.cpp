//
// Created by Echo on 25-2-16.
//

#include "RectPacking.h"

#include "Core/Base/Tree.h"

using namespace func::algo;

struct PackingRect {
  core::Rect2DI rect;
  bool free = true;
};

core::Rect2DI RectPacking::GetNextAvailableRect(core::Vector2I bound, std::span<core::Vector2I> rects) {
  core::BinaryTreeNode<PackingRect> rect_tree;
  // 设Root (0, 0, 1024, 1024)
  PackingRect root = {.rect = {.position = {0, 0}, .size = bound}};
  rect_tree.SetData(root);
  core::BinaryTreeNode<PackingRect> *current = &rect_tree;
  for (const auto &rect : rects) {
    if (!current) {
      core::Rect2DI not_enough_rect = {.position = {0, 0}, .size = {0, 0}};
      return not_enough_rect;
    }
    Int32 width = rect.x;
    Int32 height = rect.y;
    // 首先按高度划分
    core::Rect2DI current_rect = current->GetData().rect;
    core::Rect2DI free_rect;
    free_rect.position.x = current_rect.position.x;
    free_rect.position.y = current_rect.position.y + height;
    free_rect.size.x = current_rect.size.x;
    free_rect.size.y = current_rect.size.y - height;
    PackingRect right_rect = {.rect = free_rect};
    current->SetChild(1, right_rect);
    /** 插入矩形(30, 50) 首先处理height, 插入后下面是完全Free 上面仍有占用
     *                 (0, 0, 1024, 1024) Free
     *                                    \
     *                                 (0, 50, 1024, 974) Free1
     */
    core::Rect2DI occupied_rect;
    occupied_rect.position.x = current_rect.position.x;
    occupied_rect.position.y = current_rect.position.y;
    occupied_rect.size.x = current_rect.size.x;
    occupied_rect.size.y = height;
    PackingRect left_rect = {.rect = occupied_rect, .free = false};
    current->SetChild(0, left_rect);
    current->GetData().free = false;
    /** 这里仍然是在处理Height, 现在二叉树是这样的
     *                 (0, 0, 1024, 1024) Occupied
     *                /                     \
     *       (0, 0, 1024, 50)  Occupied  (0, 50, 1024, 974) Free1
     */
    current = current->GetChild(0);
    // 处理width, 首先是Free
    free_rect.position.x = current_rect.position.x + width;
    free_rect.position.y = current_rect.position.y;
    free_rect.size.x = current_rect.size.x - width;
    free_rect.size.y = current_rect.size.y;
    right_rect = {.rect = free_rect};
    current->SetChild(1, right_rect);
    /**
     *                        (0, 0, 1024, 1024) Occupied
     *                       /                     \
     *              (0, 0, 1024, 50)  Occupied  (0, 50, 974, 1024) Free1
     *                        \
     *                       (30, 0, 994, 50) Free2
     */
    occupied_rect.position.x = current_rect.position.x;
    occupied_rect.position.y = current_rect.position.y;
    occupied_rect.size.x = width;
    occupied_rect.size.y = current_rect.size.y;
    left_rect = {.rect = occupied_rect, .free = false};
    current->SetChild(0, left_rect);
    /**
     *                        (0, 0, 1024, 1024) Occupied
     *                       /                     \
     *              (0, 0, 1024, 50)  Occupied  (0, 50, 974, 1024) Free1
     *                  /            \
     *      (0, 0, 30, 50) Occupied   (30, 0, 994, 50) Free2
     */
    /**
     * 到现在 划分完成, 划分出来的空闲矩形是
     *   --------------------------------------
     *   |      |                             |
     *   |      |                             |
     *   | -----|-----------------------------|
     *   |                                    |
     *   |                                    |
     *   --------------------------------------
     *   下面的是Free1 上面的是Free2
     */
    // 如果继续插入, 则寻找最中前序遍历寻找Free, 因为这样找出的就是从上到下排列的矩形
    current = rect_tree.FindDFS([](const PackingRect &packing_rect) { return packing_rect.free; });
  }
  if (!current) {
    core::Rect2DI not_enough_rect = {.position = {0, 0}, .size = {0, 0}};
    return not_enough_rect;
  }
  core::Rect2DI result;
  result.position = current->GetData().rect.position;
  result.size = current->GetData().rect.size;
  return result;
}

core::Rect2DI RectPacking::GetNextAvailableRect(core::Vector2I bound, std::span<core::Vector2I> rects,
                                                core::Vector2I required_size) {
  core::Rect2DI rect = GetNextAvailableRect(bound, rects);
  if (rect.size.x < required_size.x || rect.size.y < required_size.y) {
    return core::Rect2DI{};
  }
  // 这时候rect已经是一个合适的矩形了, 选择required_size合适的位置和大小
  rect.size = required_size;
  return rect;
}