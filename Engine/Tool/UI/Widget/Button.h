/**
 * @file Button.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Event/Event.h"
#include "ToolCommon.h"
#include "WidgetBase.h"

WIDGET_NAMESPACE_BEGIN

struct ButtonClickEvent : public TEvent<>
{

};

class Button : public WidgetBase {
public:
    void Draw(float InDeltaTime) override;
    Button& SetText(const String& InText);

protected:
    String mButtonText = L"按钮";
    ButtonClickEvent OnButtonClicked;

private:
    AnsiString mCachedAnsiString;
};

WIDGET_NAMESPACE_END
