//
// Created by Echo on 2025/4/7.
//

#include "LayoutUtility.hpp"

Rect2Df LayoutUtility::ScaleFit(Rect2Df target, Vector2f original_size, bool horizontal_first) {
    // 处理无效输入（例如 original 宽高为0）
    if (original_size.x <= 0.0f || original_size.y <= 0.0f) {
        // 返回目标中心点 + 空尺寸
        return Rect2Df(target.pos + Vector2f{target.size.x * 0.5f, target.size.y * 0.5f}, Vector2f{0, 0});
    }

    float scale;
    if (horizontal_first) {
        // 水平优先：先尝试铺满宽度，若高度超出则改用高度适配
        float scale_w = target.size.x / original_size.x;
        float scaled_h = original_size.y * scale_w;
        if (scaled_h <= target.size.y) {
            scale = scale_w;
        } else {
            scale = target.size.y / original_size.y;
        }
    } else {
        // 垂直优先：先尝试铺满高度，若宽度超出则改用宽度适配
        float scale_h = target.size.y / original_size.y;
        float scaled_w = original_size.x * scale_h;
        if (scaled_w <= target.size.x) {
            scale = scale_h;
        } else {
            scale = target.size.x / original_size.x;
        }
    }

    // 计算缩放后的尺寸
    Vector2f new_size = {original_size.x * scale, original_size.y * scale};

    // 计算居中位置
    Vector2f new_pos = {target.pos.x + (target.size.x - new_size.x) * 0.5f, target.pos.y + (target.size.y - new_size.y) * 0.5f};

    return Rect2Df(new_pos, new_size);
}
