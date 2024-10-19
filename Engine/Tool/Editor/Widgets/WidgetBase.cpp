/**
 * @file WidgetBase.cpp
 * @author Echo 
 * @Date 24-8-31
 * @brief 
 */

#include "WidgetBase.h"

#include "ImGui/ImGuiHelper.h"

#include <imgui.h>

namespace tool::widget
{

void WidgetBase::Text(const char* str, Color foreground, Color background)
{
    if (foreground.IsValid())
    {
        ImGuiHelper::PushTextColor(foreground);
    }

    if (background.IsValid())
    {
        auto draw_pos  = ImGuiHelper::GetCursorScreenPos();
        auto text_size = ImGuiHelper::CalcTextSize(str);
        ImGuiHelper::SetCursorScreenPos(draw_pos.x + ImGuiHelper::GetFramePadding().x, draw_pos.y);
        ImGuiHelper::DrawRectFilled(draw_pos, draw_pos + text_size, background, 0);
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

}   // namespace tool::widgets
