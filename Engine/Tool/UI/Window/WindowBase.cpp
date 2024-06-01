/**
 * @file WindowBase.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#include "WindowBase.h"
#include "Utils/StringUtils.h"
#include "UI/Widget/WidgetBase.h"

#include "CoreDef.h"

#include <imgui.h>

GENERATED_SOURCE()

WINDOW_NAMESPACE_BEGIN

bool bClosed;

void WindowBase::Tick(float InDeltaTime) {
    if (bDirty) {
        mCachedAnsiWindowName = StringUtils::ToAnsiString(mWindowName);
        bDirty                = false;
    }
    if (bVisiable) {
        ImGui::Begin(mCachedAnsiWindowName.c_str(), &bClosed);
        Draw(InDeltaTime);
        ImGui::End();
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

void WindowBase::MarkDirty() {
    bDirty = true;
}

void WindowBase::Draw(float InDeltaTime) {
    for (auto Widget: mWidgets) {
        Widget->Draw(InDeltaTime);
    }
}

WINDOW_NAMESPACE_END
