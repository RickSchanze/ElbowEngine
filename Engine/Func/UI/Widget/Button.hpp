//
// Created by Echo on 2025/3/31.
//

#pragma once
#include "Func/UI/UiUtility.hpp"
#include "Widget.hpp"


class Text;

enum class ButtonState {
    Normal,
    Hovered,
    Pressed,
};

struct ButtonClickedEvent : MulticastEvent<void> {};

// 默认size.y为30 字体size.y = 20 size.x = 字体size.x + 20左右各有10的padding
class Button : public Widget {
    REFLECTED_CLASS(Button)

    ObjectPtr<Text> text_;
    Padding padding_;


protected:
    virtual void OnClick();

    Color SelectColor() const;

public:
    Button();

    void Rebuild() override;
    void Draw(rhi::CommandBuffer &cmd) override;
    Vector2f GetRebuildRequiredSize() const override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseButtonPressed(MouseButton button, Vector2f pos) override;
    void OnMouseButtonReleased(MouseButton button, Vector2f pos) override;

    ButtonClickedEvent Evt_OnClicked{};

protected:
    ButtonState state_ = ButtonState::Normal;
};

REGISTER_TYPE(Button)
