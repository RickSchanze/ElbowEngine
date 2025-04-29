//
// Created by Echo on 2025/4/7.
//

#include "LayoutUtility.hpp"

Rect2Df LayoutUtility::ScaleFit(Rect2Df InTarget, Vector2f InOriginalSize, bool InHorizontalFirst)
{
    // 处理无效输入（例如 original 宽高为0）
    if (InOriginalSize.X <= 0.0f || InOriginalSize.Y <= 0.0f)
    {
        // 返回目标中心点 + 空尺寸
        return Rect2Df(InTarget.Pos + Vector2f{InTarget.Size.X * 0.5f, InTarget.Size.Y * 0.5f}, Vector2f{0, 0});
    }

    float Scale;
    if (InHorizontalFirst)
    {
        // 水平优先：先尝试铺满宽度，若高度超出则改用高度适配
        const float ScaledW = InTarget.Size.X / InOriginalSize.X;
        const float ScaledH = InOriginalSize.Y * ScaledW;
        if (ScaledH <= InTarget.Size.Y)
        {
            Scale = ScaledW;
        }
        else
        {
            Scale = InTarget.Size.Y / InOriginalSize.Y;
        }
    }
    else
    {
        // 垂直优先：先尝试铺满高度，若宽度超出则改用宽度适配
        const float ScaledH = InTarget.Size.Y / InOriginalSize.Y;
        const float ScaledW = InOriginalSize.X * ScaledH;
        if (ScaledW <= InTarget.Size.X)
        {
            Scale = ScaledH;
        }
        else
        {
            Scale = InTarget.Size.X / InOriginalSize.X;
        }
    }

    // 计算缩放后的尺寸
    const Vector2f NewSize = {InOriginalSize.X * Scale, InOriginalSize.Y * Scale};

    // 计算居中位置
    const Vector2f NewPos = {InTarget.Pos.X + (InTarget.Size.X - NewSize.X) * 0.5f, InTarget.Pos.Y + (InTarget.Size.Y - NewSize.Y) * 0.5f};

    return Rect2Df(NewPos, NewSize);
}
