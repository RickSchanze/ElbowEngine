/**
 * @file WindowCommon.h
 * @author Echo 
 * @Date 24-4-20
 * @brief 
 */

#pragma once

#define IMGUI_FONT_STR U8("帧引擎渲染轴栈")

#define GOOGLE_MATERIAL_ICON_FONT_PATH L"Fonts/MaterialIcons-Regular.ttf"
#define EDITOR_UI_FONT_PATH L"Fonts/Consolas-with-Yahei_Nerd_Font.ttf"
#define DEFAULT_FONT_SIZE 20

inline float g_window_scale = 1.5f;

#define WINDOW_SCALE(x) ((x) * g_window_scale)
