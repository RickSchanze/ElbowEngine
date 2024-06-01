/**
 * @file WindowBase.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Object/Object.h"
#include "ToolCommon.h"
#include "WindowBase.generated.h"

namespace Tool::Widget {
class WidgetBase;
}

WINDOW_NAMESPACE_BEGIN

class REFL WindowBase : public Object {

private:
    RTTR_REGISTRATION_FRIEND

public:
    void Tick(float InDeltaTime);

    // 在这个函数里添加所有的Widget
    virtual void Construct() {}

    ~WindowBase() override;

    void AddWidget(Widget::WidgetBase* Widget) { mWidgets.push_back(Widget); }

    WindowBase& SetWindowName(const String& InWindowName);

    WindowBase& SetVisible(bool bVisible) {
        bVisiable = bVisible;
        return *this;
    }

protected:
    void MarkDirty();

    virtual void Draw(float InDeltaTime);

    PROPERTY(Serialized, Name = "WindowName")
    String mWindowName;

    AnsiString mCachedAnsiWindowName;

    TArray<Widget::WidgetBase*> mWidgets;

    bool bDirty = true;

    bool bVisiable = true;
};

WINDOW_NAMESPACE_END
