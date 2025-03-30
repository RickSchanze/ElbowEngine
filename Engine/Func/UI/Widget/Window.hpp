//
// Created by Echo on 2025/3/29.
//

#pragma once
#include "Core/Object/ObjectPtr.hpp"
#include "Widget.hpp"


class Text;
class Window : public Widget {
    REFLECTED_CLASS(Window)
public:
    explicit Window();
    ~Window() override;

    void Rebuild() override;
    void Draw(rhi::CommandBuffer &cmd) override;
    Vector2f GetRebuildRequiredSize() override;
    Widget *GetSlotWidget() const { return slot_; }
    void SetSlotWidget(Widget *now);

    bool IsFocused() const { return focused_; }
    void SetFocused(bool now);

protected:
    ObjectPtr<Text> title_text_;
    ObjectPtr<Widget> slot_;
    bool focused_ = false;
};

REGISTER_TYPE(Window)
