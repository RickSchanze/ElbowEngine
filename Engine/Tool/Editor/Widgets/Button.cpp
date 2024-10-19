/**
 * @file Button.cpp
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#include "Button.h"

#include "ImGui/ImGuiHelper.h"

namespace tool::widget
{

Button::Button()
{
    normal_color  = ImGuiHelper::GetButtonNormalColor();
    hovered_color = ImGuiHelper::GetButtonHoveredColor();
    pressed_color = ImGuiHelper::GetButtonPressedColor();
    text_color    = Color::Invalid();
}

void Button::Draw()
{
    if (text_)
    {
        switch (state_)
        {
        case EButtonState::Hovered: Text(text_.ToCStyleString(), text_color, hovered_color); break;
        case EButtonState::Pressed: Text(text_.ToCStyleString(), text_color, pressed_color); break;
        case EButtonState::Normal: Text(text_.ToCStyleString(), text_color, normal_color); break;
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
                if (OnClick)
                {
                    OnClick();
                }
            }
        }
        else
        {
            state_ = EButtonState::Normal;
        }

        if (tooltip_text_)
        {
            ImGuiHelper::SetItemTooltip(tooltip_text_);
        }
    }
}

Button& Button::SetText(const CachedString& str)
{
    if (text_ != str) text_ = str;
    return *this;
}

Button& Button::SetTextColor(const Color& color)
{
    if (color.IsValid())
    {
        text_color = color;
    }
    return *this;
}

Button& Button::SetTooltipText(const CachedString& str)
{
    if (tooltip_text_ != str)
    {
        tooltip_text_ = str;
    }
    return *this;
}

Button& Button::SetEventOnClick(const Function<void()>& func)
{
    OnClick = func;
    return *this;
}

EButtonState Button::GetState() const
{
    return state_;
}

}   // namespace tool::widgets
