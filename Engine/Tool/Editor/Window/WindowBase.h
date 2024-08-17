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

namespace Tool::Widget
{
class WidgetBase;
}

WINDOW_NAMESPACE_BEGIN

enum class REFL EWindowVisiable
{
    Visiable,
    Hidden,
    DefaultMax
};

struct WindowVisiableChangedEvent : TEvent<EWindowVisiable, EWindowVisiable>
{
};

class REFL WindowBase : public Object
{
    GENERATED_BODY(WindowBase)

public:
    WindowBase() : Super(EOF_IsWindow) {}

    void Tick(float delta_time);

    void Construct();

    ~WindowBase() override = default;

    void AddWidget(Widget::WidgetBase* Widget) { widgets_.push_back(Widget); }

    void          SetWindowName(const String& InWindowName);
    const String& GetWindowName() const { return window_name_; }

    WindowBase& SetVisible(EWindowVisiable InVisible);

    bool IsVisible() const { return visiable_ == EWindowVisiable::Visiable; }

    EWindowVisiable GetVisible() const { return visiable_; }

    bool IsValid() const override;

    bool HasConstructed() const { return constructed_; }

    bool IsSingleton() const { return singleton_; }

public:
    // 当窗口可见性发生变化时触发，参数是旧的可见性
    WindowVisiableChangedEvent OnVisiableChanged;

protected:
    void MarkDirty();

    virtual void Draw(float delta_time);

    PROPERTY(Serialized, Getter = GetWindowName, Setter = SetWindowName)
    String window_name_;

    AnsiString cached_ansi_window_name_;

    TArray<Widget::WidgetBase*> widgets_;

    bool dirty_       = true;
    bool constructed_ = false;

    EWindowVisiable visiable_ = EWindowVisiable::DefaultMax;

    int width_  = 0;
    int height_ = 0;

    // 这个窗口只能存在一个
    bool singleton_ = true;

private:
    bool imgui_show_window_ = true;
};

WINDOW_NAMESPACE_END
