/**
 * @file WindowBase.h
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Event/Event.h"
#include "Object/Object.h"
#include "ToolCommon.h"

#include "WindowBase.generated.h"

namespace Tool::Widget {
class WidgetBase;
}

WINDOW_NAMESPACE_BEGIN

enum class REFL EWindowVisiable { Visiable, Hidden, DefaultMax };

struct WindowVisiableChangedEvent : TEvent<EWindowVisiable>
{};

class REFL WindowBase : public Object {
    GENERATED_BODY(WindowBase)

public:
    WindowBase() : Super(EOF_IsWindow) {}

    void Tick(float InDeltaTime);

    // 在这个函数里添加所有的Widget
    virtual void Construct();

    ~WindowBase() override;

    void AddWidget(Widget::WidgetBase* Widget) { mWidgets.push_back(Widget); }

    WindowBase& SetWindowName(const String& InWindowName);

    // 设置窗口可见性
    FUNCTION()
    WindowBase& SetVisible(EWindowVisiable InVisible);

    FUNCTION()
    bool IsValid() const override;

    FUNCTION()
    bool HasConstructed() const { return bConstructed; }

public:
    // 当窗口可见性发生变化时触发，参数是旧的可见性
    WindowVisiableChangedEvent OnVisiableChanged;

protected:
    void MarkDirty();

    virtual void Draw(float InDeltaTime);

    PROPERTY(Serialized, Name = "WindowName")
    String mWindowName;

    AnsiString mCachedAnsiWindowName;

    TArray<Widget::WidgetBase*> mWidgets;

    bool bDirty       = true;
    bool bConstructed = false;

    EWindowVisiable mVisiable = EWindowVisiable::DefaultMax;

private:
    bool mImguiShowWindow = true;
};

WINDOW_NAMESPACE_END
