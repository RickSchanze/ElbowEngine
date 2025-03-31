//
// Created by Echo on 2025/3/30.
//

#pragma once
#include "Layout.hpp"

/**
 * VerticalLayout的ui rect的size会被初始化为0
 * 这表示要求父Widget铺满, 如果不为0, 则会尝试按照要求的尺寸进行布局
 */
class VerticalLayout : public Layout {
    REFLECTED_CLASS(VerticalLayout)

    Array<ObjectPtr<Widget>> children_;
    EVerticalLayout layout_ = EVerticalLayout::Top;
    Float space{5};

    Widget* entered_widget_ = nullptr;

public:
    VerticalLayout();

    void OnMouseMove(Vector2f old, Vector2f now) override;
    void OnMouseButtonReleased(MouseButton button, Vector2f pos) override;
    void OnMouseButtonPressed(MouseButton button, Vector2f pos) override;

    Vector2f GetRebuildRequiredSize() const override;
    void Rebuild() override;
    void RebuildHierarchy() override;
    void Draw(rhi::CommandBuffer &cmd) override;

    void AddChild(Widget* w);
    void RemoveChild(Widget* w);
    void RemoveChild(Int32 i);
};

REGISTER_TYPE(VerticalLayout)
