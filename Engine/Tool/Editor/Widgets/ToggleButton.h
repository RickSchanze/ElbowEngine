/**
 * @file ToggleButton.h
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#pragma once
#include "Button.h"

namespace tool::widget {

class ToggleButton : public Button {
public:
    explicit ToggleButton(bool default_toggle_on = false);

    void Draw() override;

    Color toggle_color_on;
    Color toggle_color_off;

    ToggleButton& SetEventOnToggleOn(const Function<void()>& on_toggle_on);
    ToggleButton& SetEventOnToggleOff(Function<void()> on_toggle_off);


protected:
    // TODO: 改成事件
    Function<void()> OnToggleOn;
    Function<void()> OnToggleOff;

    EButtonState previous_state_;
};

}
