/**
 * @file WidgetBase.h
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#pragma once
#include "Math/MathTypes.h"
#include "ToolCommon.h"

WIDGET_NAMESPACE_BEGIN

class WidgetBase
{
public:
    virtual ~WidgetBase() = default;
    virtual void Draw() = 0;

    void Text(const char* str, Color foreground = Color::Invalid(), Color background = Color::Invalid());
    void Text(const AnsiString& str, Color foreground = Color::Invalid(), Color background = Color::Invalid());
};

WIDGET_NAMESPACE_END
