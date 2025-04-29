//
// Created by Echo on 2025/3/27.
//
#include "Algo.hpp"

#include "Core/Collection/Tree.hpp"
#include "Core/Profile.hpp"

using namespace algo;

Rect2Di RectPacking::GetNextAvailableRect(const Vector2i InBound, const Span<Vector2i> InRects)
{
    ProfileScope _(__func__);
    struct PackingRect
    {
        Rect2Di Rect;
        bool Free = true;
    };
    BinaryTreeNode<PackingRect> RectTree;
    // 设Root (0, 0, 1024, 1024)
    PackingRect Root = {.Rect = {.Pos = {0, 0}, .Size = InBound}};
    RectTree.SetData(Root);
    BinaryTreeNode<PackingRect>* Current = &RectTree;
    for (const auto& Rect : InRects)
    {
        if (!Current)
        {
            Rect2Di NotEnoughRect = {.Pos = {0, 0}, .Size = {0, 0}};
            return NotEnoughRect;
        }
        Int32 Width = Rect.X;
        Int32 Height = Rect.Y;
        // 首先按高度划分
        Rect2Di CurrentRect = Current->GetData().Rect;
        Rect2Di FreeRect;
        FreeRect.Pos.X = CurrentRect.Pos.X;
        FreeRect.Pos.Y = CurrentRect.Pos.Y + Height;
        FreeRect.Size.X = CurrentRect.Size.X;
        FreeRect.Size.Y = CurrentRect.Size.Y - Height;
        PackingRect RightRect = {.Rect = FreeRect};
        Current->SetChild(1, RightRect);
        /** 插入矩形(30, 50) 首先处理height, 插入后下面是完全Free 上面仍有占用
         *                 (0, 0, 1024, 1024) Free
         *                                    \
         *                                 (0, 50, 1024, 974) Free1
         */
        Rect2Di OccupiedRect;
        OccupiedRect.Pos.X = CurrentRect.Pos.X;
        OccupiedRect.Pos.Y = CurrentRect.Pos.Y;
        OccupiedRect.Size.X = CurrentRect.Size.X;
        OccupiedRect.Size.Y = Height;
        PackingRect LeftRect = {.Rect = OccupiedRect, .Free = false};
        Current->SetChild(0, LeftRect);
        Current->GetData().Free = false;
        /** 这里仍然是在处理Height, 现在二叉树是这样的
         *                 (0, 0, 1024, 1024) Occupied
         *                /                     \
         *       (0, 0, 1024, 50)  Occupied  (0, 50, 1024, 974) Free1
         */
        Current = Current->GetChild(0);
        // 处理width, 首先是Free
        FreeRect.Pos.X = CurrentRect.Pos.X + Width;
        FreeRect.Pos.Y = CurrentRect.Pos.Y;
        FreeRect.Size.X = CurrentRect.Size.X - Width;
        FreeRect.Size.Y = CurrentRect.Size.Y;
        RightRect = {.Rect = FreeRect};
        Current->SetChild(1, RightRect);
        /**
         *                        (0, 0, 1024, 1024) Occupied
         *                       /                     \
         *              (0, 0, 1024, 50)  Occupied  (0, 50, 974, 1024) Free1
         *                        \
         *                       (30, 0, 994, 50) Free2
         */
        OccupiedRect.Pos.X = CurrentRect.Pos.X;
        OccupiedRect.Pos.Y = CurrentRect.Pos.Y;
        OccupiedRect.Size.X = Width;
        OccupiedRect.Size.Y = CurrentRect.Size.Y;
        LeftRect = {.Rect = OccupiedRect, .Free = false};
        Current->SetChild(0, LeftRect);
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
        Current = RectTree.FindDFS([](const PackingRect& InPackingRect) { return InPackingRect.Free; });
    }
    if (!Current)
    {
        constexpr Rect2Di NotEnoughRect = {.Pos = {0, 0}, .Size = {0, 0}};
        return NotEnoughRect;
    }
    Rect2Di Result;
    Result.Pos = Current->GetData().Rect.Pos;
    Result.Size = Current->GetData().Rect.Size;
    return Result;
}

Rect2Di RectPacking::GetNextAvailableRect(const Vector2i InBound, const Span<Vector2i> InRects, const Vector2i InRequiredSize)
{
    ProfileScope _(__func__);
    Rect2Di Rect = GetNextAvailableRect(InBound, InRects);
    if (Rect.Size.X < InRequiredSize.X || Rect.Size.Y < InRequiredSize.Y)
    {
        return Rect2Di{};
    }
    // 这时候rect已经是一个合适的矩形了, 选择required_size合适的位置和大小
    Rect.Size = InRequiredSize;
    return Rect;
}
