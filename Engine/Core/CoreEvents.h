/**
 * @file CoreEvents.h
 * @author Echo 
 * @Date 24-6-11
 * @brief 
 */

#pragma once

#include "Event/Event.h"

// clang-format off

struct ProjectPathSetEvent : TEvent<> {};
struct AppExitEvent : TEvent<> {};
struct AppWindowResizeEvent : TEvent<int32_t, int32_t> {};
struct AppNeedWaitEvent : TEvent<> {};
struct GetAppWindowSizeEvent : TEvent<int*, int *> {};

// clang-format on

inline ProjectPathSetEvent   OnProjectPathSet;
inline AppExitEvent          OnAppExit;
inline AppWindowResizeEvent  OnAppWindowResized;
inline AppNeedWaitEvent      OnAppNeedWait;
inline GetAppWindowSizeEvent OnGetAppWindowSize;
