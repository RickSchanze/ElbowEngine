/**
 * @file WidgetBase.cpp
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#include "WidgetBase.h"

#include "ImGui/ImGuiHelper.h"

#include <imgui.h>

WIDGET_NAMESPACE_BEGIN

void WidgetBase::Text(const char* str, Color foreground, Color background)
{
    if (foreground.IsValid())
    {
        ImGuiHelper::PushTextColor(foreground);
    }

    if (background.IsValid())
    {
        auto list      = ImGui::GetWindowDrawList();
        auto draw_pos  = ImGuiHelper::GetCursorScreenPos();
        auto text_size = ImGuiHelper::CalcTextSize(str);
        ImGuiHelper::SetCursorScreenPos(draw_pos.x + ImGuiHelper::GetFramePadding().x, draw_pos.y);
        list->AddRectFilled(draw_pos, draw_pos + text_size, static_cast<ImU32>(background));
    }

    ImGuiHelper::Text(str);

    if (foreground.IsValid())
    {
        ImGuiHelper::PopColor();
    }
}

void WidgetBase::Text(const AnsiString& str, Color foreground, Color background)
{
    Text(str.c_str(), foreground, background);
}

WIDGET_NAMESPACE_END
