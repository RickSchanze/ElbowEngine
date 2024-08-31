/**
 * @file ToggleButton.h
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#pragma once
#include "Button.h"

WIDGET_NAMESPACE_BEGIN

class ToggleButton : public Button {
public:
    explicit ToggleButton(bool default_toggle_on = false);

    void Draw() override;

    Color toggle_color_on;
    Color toggle_color_off;

    ToggleButton& SetEventOnToggleOn(const TFunction<void()>& on_toggle_on);
    ToggleButton& SetEventOnToggleOff(TFunction<void()> on_toggle_off);


protected:
    // TODO: 改成事件
    TFunction<void()> OnToggleOn;
    TFunction<void()> OnToggleOff;

    EButtonState previous_state_;
};

WIDGET_NAMESPACE_END
