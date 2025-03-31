//
// Created by Echo on 2025/3/29.
//

#pragma once
#include "Core/Object/ObjectPtr.hpp"
#include "Widget.hpp"

constexpr Float DEFAULT_WINDOW_TITLE_HEIGHT = 30.f;

class Text;
class Window : public Widget {
    REFLECTED_CLASS(Window)
public:
    explicit Window();
    ~Window() override;

    void Rebuild() override;
    void RebuildHierarchy();
    void Draw(rhi::CommandBuffer &cmd) override;
    Vector2f GetRebuildRequiredSize() const override;
    Widget *GetSlotWidget() const { return slot_; }
    void SetSlotWidget(Widget *now);

    bool IsFocused() const { return focused_; }
    void SetFocused(bool now);

    void OnMouseMove(Vector2f old, Vector2f now) override;
    void OnMouseButtonPressed(MouseButton button, Vector2f pos) override;
    void OnMouseButtonReleased(MouseButton button, Vector2f pos) override;

protected:
    ObjectPtr<Text> title_text_;
    ObjectPtr<Widget> slot_;
    bool focused_ = false;
    bool moving_window_ = false;
};

REGISTER_TYPE(Window)
