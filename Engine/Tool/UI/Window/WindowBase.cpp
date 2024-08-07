/**
 * @file WindowBase.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#include "WindowBase.h"
#include "CoreDef.h"
#include "EngineApplication.h"
#include "Utils/StringUtils.h"
#include <imgui.h>

#include "WindowBase.generated.h"

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

void WindowBase::Tick(float delta_time)
{
    if (dirty_)
    {
        cached_ansi_window_name_ = StringUtils::ToAnsiString(window_name_);
        dirty_                   = false;
    }

    if (imgui_show_window_)
    {
        if (!ImGui::Begin(cached_ansi_window_name_.c_str(), &imgui_show_window_))
        {
            ImGui::End();
            SetVisible(EWindowVisiable::Hidden);
        }
        else
        {
            Draw(delta_time);
            ImGui::End();
        }
    }
    else
    {
        SetVisible(EWindowVisiable::Hidden);
    }
}
void WindowBase::Construct()
{
    if (constructed_)
    {
        LOG_WARNING_CATEGORY(UI, L"窗口只能被初始化一次");
        return;
    }
    else
    {
        constructed_ = true;
    }
}

void WindowBase::SetWindowName(const String& InWindowName)
{
    window_name_ = InWindowName;
    MarkDirty();
}

WindowBase& WindowBase::SetVisible(EWindowVisiable InVisible)
{
    if (InVisible != visiable_)
    {
        auto OldVisiable = visiable_;
        visiable_        = InVisible;

        if (visiable_ == EWindowVisiable::Visiable)
        {
            EngineApplication::Get().AddWindow(this);
            imgui_show_window_ = true;
        }
        else if (visiable_ == EWindowVisiable::Hidden)
        {
            EngineApplication::Get().RemoveWindow(this);
            imgui_show_window_ = false;
        }

        OnVisiableChanged.Broadcast(OldVisiable);
    }
    return *this;
}
bool WindowBase::IsValid() const
{
    return Super::IsValid() && constructed_;
}

void WindowBase::MarkDirty()
{
    dirty_ = true;
}

void WindowBase::Draw(float InDeltaTime)
{
    ImGui::Text(U8("未实现"));
}

WINDOW_NAMESPACE_END
