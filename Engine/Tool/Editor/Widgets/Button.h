/**
 * @file Button.h
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#pragma once
#include "CachedString.h"
#include "WidgetBase.h"

namespace Tool::Widgets
{

enum class EButtonState
{
    Hovered,
    Pressed,
    Normal,
    ToggleOn,
    ToggleOff
};

class Button : public WidgetBase
{
public:
    Button();

    void Draw() override;

    Button& SetText(const CachedString& str);
    Button& SetTextColor(const Color& color);
    Button& SetTooltipText(const CachedString& str);
    Button& SetEventOnClick(const TFunction<void()>& func);

    EButtonState GetState() const;

    Color normal_color;
    Color hovered_color;
    Color pressed_color;
    Color text_color;



protected:
    EButtonState state_ = EButtonState::Normal;
    CachedString text_;
    CachedString tooltip_text_;

    // TODO: 改成事件
    TFunction<void()> OnClick;
};

}   // namespace Tool::Widget
