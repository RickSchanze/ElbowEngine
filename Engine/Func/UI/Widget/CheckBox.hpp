//
// Created by Echo on 2025/3/31.
//

#pragma once
#include "Widget.hpp"

struct CheckBoxCheckedChangeEvent : MulticastEvent<void, bool /* now */> {};

class Text;
class CheckBox : public Widget {
    REFLECTED_CLASS(CheckBox)
    // Reflected members
    ObjectPtr<Text> text_;
    bool checked_ = false;

public:
    CheckBox();

    void Rebuild() override;
    Vector2f GetRebuildRequiredSize() const override;
    void Draw(rhi::CommandBuffer &cmd) override;
    void OnMouseButtonPressed(MouseButton button, Vector2f pos) override;
    void OnMouseButtonReleased(MouseButton button, Vector2f pos) override;
    void OnMouseLeave() override;
    bool IsChecked() const { return checked_; }
    void SetChecked(bool now);

    Text *GetText() const { return text_; }
    void SetText(StringView text);

    CheckBoxCheckedChangeEvent Evt_OnCheckedChanged{};

private:
    bool checkbox_icon_pressed = false;
};

REGISTER_TYPE(CheckBox)
