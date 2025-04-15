//
// Created by Echo on 2025/4/7.
//

#include "LayoutUtility.hpp"

Rect2Df LayoutUtility::ScaleFit(Rect2Df target, Vector2f original_size, bool horizontal_first) {
    // 处理无效输入（例如 original 宽高为0）
    if (original_size.X <= 0.0f || original_size.Y <= 0.0f) {
        // 返回目标中心点 + 空尺寸
        return Rect2Df(target.pos + Vector2f{target.size.X * 0.5f, target.size.Y * 0.5f}, Vector2f{0, 0});
    }

    float scale;
    if (horizontal_first) {
        // 水平优先：先尝试铺满宽度，若高度超出则改用高度适配
        float scale_w = target.size.X / original_size.X;
        float scaled_h = original_size.Y * scale_w;
        if (scaled_h <= target.size.Y) {
            scale = scale_w;
        } else {
            scale = target.size.Y / original_size.Y;
        }
    } else {
        // 垂直优先：先尝试铺满高度，若宽度超出则改用宽度适配
        float scale_h = target.size.Y / original_size.Y;
        float scaled_w = original_size.X * scale_h;
        if (scaled_w <= target.size.X) {
            scale = scale_h;
        } else {
            scale = target.size.X / original_size.X;
        }
    }

    // 计算缩放后的尺寸
    Vector2f new_size = {original_size.X * scale, original_size.Y * scale};

    // 计算居中位置
    Vector2f new_pos = {target.pos.X + (target.size.X - new_size.X) * 0.5f, target.pos.Y + (target.size.Y - new_size.Y) * 0.5f};

    return Rect2Df(new_pos, new_size);
}
