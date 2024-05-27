/**
 * @file WindowBase.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "ToolCommon.h"
namespace Tool::Widget {
class WidgetBase;
}

WINDOW_NAMESPACE_BEGIN

class WindowBase {
public:
    void Tick(float InDeltaTime);

    // 在这个函数里添加所有的Widget
    virtual void Construct() {}

    virtual ~WindowBase();

    void AddWidget(Widget::WidgetBase* Widget) { mWidgets.push_back(Widget); }

    WindowBase& SetWindowName(const String& InWindowName);

    WindowBase& SetVisible(bool bVisible) {
        bVisiable = bVisible;
        return *this;
    }

protected:
    void MarkDirty();

    virtual void Draw(float InDeltaTime);

    String                      mWindowName;
    AnsiString                  mCachedAnsiWindowName;
    TArray<Widget::WidgetBase*> mWidgets;

    bool bDirty    = true;
    bool bVisiable = true;
};

WINDOW_NAMESPACE_END
