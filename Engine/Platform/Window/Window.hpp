//
// Created by Echo on 2025/3/23.
//

#pragma once

#include "PlatformWindow.hpp"

/**
 * 创建一个Window实例
 * @param window_lib window使用的窗口库
 * @param title 窗口标题 没有的话从PlatformConfig里取
 * @param width 窗口宽度 =0 则从PlatformConfig里取
 * @param height 窗口高度 =0 则从PlatformConfig里取
 * @param flags 窗口标志 =0 则从PlatformConfig里取
 * @return
 */
PlatformWindow *CreatePlatformWindow(WindowLib window_lib = WindowLib::Count, StringView title = "", UInt32 width = 0, UInt32 height = 0,
                                     Int32 flags = 0);
