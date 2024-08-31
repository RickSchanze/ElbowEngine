/**
 * @file ToggleButton.cpp
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#include "ToggleButton.h"

#include "ImGui/ImGuiHelper.h"

WIDGET_NAMESPACE_BEGIN

ToggleButton::ToggleButton(bool default_toggle_on)
{
    toggle_color_on  = Color(0.3f, 0.3f, 0.3f);
    toggle_color_off = Color(0.1f, 0.1f, 0.1f);
    hovered_color = Color(0.4f, 0.4f,0.4f);
    pressed_color = Color(0.6f, 0.6f, 0.6f);
    if (default_toggle_on)
    {
        state_ = previous_state_ = EButtonState::ToggleOn;
    }
    else
    {
        state_ = previous_state_ = EButtonState::ToggleOff;
    }
}

void ToggleButton::Draw()
{
    if (text_)
    {
        switch (state_)
        {
        case EButtonState::Hovered: Text(text_.ToCStyleString(), text_color, hovered_color); break;
        case EButtonState::Pressed: Text(text_.ToCStyleString(), text_color, pressed_color); break;
        case EButtonState::Normal: LOG_ERROR_CATEGORY(Widget, L"ToggleButton不应该进入这个状态"); break;
        case EButtonState::ToggleOn: Text(text_.ToCStyleString(), text_color, toggle_color_on); break;
        case EButtonState::ToggleOff: Text(text_.ToCStyleString(), text_color, toggle_color_off); break;
        }
        if (ImGuiHelper::IsItemHovered())
        {
            state_ = EButtonState::Hovered;
            if (ImGuiHelper::IsLeftMouseDown())
            {
                state_ = EButtonState::Pressed;
            }
            if (ImGuiHelper::IsLeftMouseReleased())
            {
                if (previous_state_ == EButtonState::ToggleOn)
                {
                    state_ = previous_state_ = EButtonState::ToggleOff;
                    if (OnToggleOff) OnToggleOff();
                }
                else
                {
                    state_ = previous_state_ = EButtonState::ToggleOn;
                    if (OnToggleOn) OnToggleOn();
                }
            }
        }
        else
        {
            state_ = previous_state_;
        }

        if (tooltip_text_)
        {
            ImGuiHelper::SetItemTooltip(tooltip_text_);
        }
    }
}

ToggleButton& ToggleButton::SetEventOnToggleOn(const TFunction<void()>& on_toggle_on)
{
    OnToggleOn = on_toggle_on;
    return *this;
}

ToggleButton& ToggleButton::SetEventOnToggleOff(TFunction<void()> on_toggle_off)
{
    OnToggleOff = on_toggle_off;
    return *this;
}

WIDGET_NAMESPACE_END
