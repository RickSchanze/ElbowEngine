/**
 * @file Button.cpp
 * @author Echo 
 * @Date 24-5-27
 * @brief 
 */

#include "Button.h"

#include "Utils/StringUtils.h"
#include <imgui.h>

WIDGET_NAMESPACE_BEGIN
void Button::Draw(float InDeltaTime) {
    bool bClicked = false;
    if (mbDirty) {
        mCachedAnsiString = StringUtils::ToAnsiString(mButtonText);
        mbDirty           = false;
    }
    if (mCachedAnsiString.empty()) {
        if (mButtonText.empty()) {
            bClicked = ImGui::Button("");
        } else {
            mCachedAnsiString = StringUtils::ToAnsiString(mButtonText);
            bClicked          = ImGui::Button(mCachedAnsiString.c_str());
        }
    } else {
        bClicked = ImGui::Button(mCachedAnsiString.c_str());
    }
    if (bClicked) {
        OnButtonClicked.Broadcast();
    }
}

Button& Button::SetText(const String& InText) {
    mButtonText = InText;
    MarkDirty();
    return *this;
}

WIDGET_NAMESPACE_END
