/**
 * @file CoreMacro.h
 * @author Echo 
 * @Date 24-9-12
 * @brief 
 */

#pragma once

// 各项功能开关, 要取消功能, 则注释掉对应的宏

// 使用Editor?
#define WITH_EDITOR

// 使用Profiler? 注意: Profiler会影响性能, 如果不连接Server还可能内存泄露
// #define ENABLE_PROFILING

// 启用测试特定代码?
#define ENABLE_TEST

// 开启ImGui?
#define USE_IMGUI 1
