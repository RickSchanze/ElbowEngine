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

namespace tool::widget
{
class WidgetBase;
}

namespace tool::window
{

EENUM()
enum class EWindowVisibility
{
    Visible,
    Hidden,
    DefaultMax
};

struct WindowVisibilityChangedEvent : TEvent<EWindowVisibility, EWindowVisibility>
{
};

ECLASS()
class WindowBase : public Object
{
    GENERATED_BODY(WindowBase)

public:
    WindowBase() : Super(EOF_IsWindow) {}

    void Tick(float delta_time);

    virtual void Construct();

    ~WindowBase() override = default;

    void AddWidget(widget::WidgetBase* Widget) { widgets_.push_back(Widget); }

    void          SetWindowName(const String& InWindowName);
    const String& GetWindowName() const { return window_name_; }

    WindowBase& SetVisible(EWindowVisibility InVisible);

    bool IsVisible() const { return visible_ == EWindowVisibility::Visible; }

    EWindowVisibility GetVisible() const { return visible_; }

    bool IsValid() const override;

    bool HasConstructed() const { return constructed_; }

    bool IsSingleton() const { return singleton_; }

public:
    // 当窗口可见性发生变化时触发，参数是旧的可见性
    WindowVisibilityChangedEvent OnVisibilityChanged;

protected:
    void MarkDirty();

    virtual void Draw(float delta_time);

    EPROPERTY(Getter = GetWindowName, Setter = SetWindowName)
    String window_name_;

    AnsiString cached_ansi_window_name_;

    Array<widget::WidgetBase*> widgets_;

    bool dirty_       = true;
    bool constructed_ = false;

    EWindowVisibility visible_ = EWindowVisibility::DefaultMax;

    int width_  = 0;
    int height_ = 0;

    // 这个窗口只能存在一个
    bool singleton_ = true;

private:
    bool imgui_show_window_ = true;
};

}
