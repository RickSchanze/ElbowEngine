/**
 * @file ImGuiHelper.h
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#pragma once

#include "CoreDef.h"

#if USE_IMGUI
#include <imgui.h>
#endif

class CachedString;
/**
 * ImGui wrapper
 * 项目里所有对ImGui的绘制调用走这个函数
 * 目的：实现ImGui开关
 */
class ImGuiHelper {
public:
    template <typename... Args>
    static void Text(const char* fmt, Args&&... args)
    {
#if USE_IMGUI
        ImGui::Text(fmt, Forward<Args>(args)...);
#endif
    }

    static void Text(CachedString& str);

    static bool CollapsingHeader(const char* label);
};
