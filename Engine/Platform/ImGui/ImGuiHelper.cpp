/**
 * @file ImGuiHelper.cpp
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#include "ImGuiHelper.h"

#include "CachedString.h"

void ImGuiHelper::Text(CachedString& str)
{
    ImGui::Text(str.ToCStyleString());
}

bool ImGuiHelper::CollapsingHeader(const char* label)
{
    return ImGui::CollapsingHeader(label);
}
