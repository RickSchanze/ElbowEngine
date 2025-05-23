//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Math/Vector.hpp"
#include "Core/Math/Rect.hpp"

class LayoutUtility {
public:
    /**
     * 将矩形范围original按比例缩放至矩形范围bigger内
     * original可比target大也可以比target小
     * @param InTarget
     * @param InOriginalSize
     * @param InHorizontalFirst 水平优先, 也就是会尽力铺满target的宽度
     * @return
     */
    static Rect2Df ScaleFit(Rect2Df InTarget, Vector2f InOriginalSize, bool InHorizontalFirst = true);
};
