/**
 * @file Button.cpp
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#include "Button.h"

void Tool::Widget::Button::Draw()
{
    if (text_)
    {
        Text(text_.ToCStyleString(), Color::SkyBlue(), Color::Red());
    }
}

void Tool::Widget::Button::SetText(const CachedString& str)
{
    text_ = str;
}
