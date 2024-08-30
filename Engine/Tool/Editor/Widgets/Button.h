/**
 * @file Button.h
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#pragma once
#include "CachedString.h"
#include "WidgetBase.h"

namespace Tool::Widget
{

enum class EButtonState
{
    Hovered,
    Pressed,
    Normal,
};

class Button : public Widgets::WidgetBase
{
public:
    void Draw() override;

    void SetText(const CachedString& str);

protected:
    EButtonState state_ = EButtonState::Normal;
    CachedString text_;
};

}   // namespace Tool::Widget
