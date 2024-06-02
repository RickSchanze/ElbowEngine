/**
 * @file WindowBase.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#include "WindowBase.h"
#include "CoreDef.h"
#include "EngineApplication.h"
#include "UI/Widget/WidgetBase.h"
#include "Utils/StringUtils.h"
#include <imgui.h>

RTTR_REGISTRATION{GENERATED_SOURCE_WindowBase_h}

WINDOW_NAMESPACE_BEGIN

    bool bClosed;

void WindowBase::Tick(float InDeltaTime) {
    if (bDirty) {
        mCachedAnsiWindowName = StringUtils::ToAnsiString(mWindowName);
        bDirty                = false;
    }

    ImGui::Begin(mCachedAnsiWindowName.c_str(), &bClosed);
    Draw(InDeltaTime);
    ImGui::End();
}
void WindowBase::Construct() {
    if (bConstructed) {
        LOG_WARNING_CATEGORY(UI, L"窗口只能被初始化一次");
        return;
    }
}

WindowBase::~WindowBase() {
    for (auto Widget: mWidgets) {
        delete Widget;
    }
}

WindowBase& WindowBase::SetWindowName(const String& InWindowName) {
    mWindowName = InWindowName;
    MarkDirty();
    return *this;
}

WindowBase& WindowBase::SetVisible(EWindowVisiable InVisible) {
    if (InVisible != mVisiable) {
        auto OldVisiable = mVisiable;
        mVisiable        = InVisible;
        if (mVisiable == EWindowVisiable::Visiable) {
            EngineApplication::Get().AddWindow(this);
        } else if (mVisiable == EWindowVisiable::Hidden) {
            EngineApplication::Get().RemoveWindow(this);
        }

        OnVisiableChanged.Broadcast(OldVisiable);
    }
    return *this;
}
bool WindowBase::IsValid() const {
    return Super::IsValid() && bConstructed;
}

void WindowBase::MarkDirty() {
    bDirty = true;
}

void WindowBase::Draw(float InDeltaTime) {
    for (auto Widget: mWidgets) {
        Widget->Draw(InDeltaTime);
    }
}

WINDOW_NAMESPACE_END
