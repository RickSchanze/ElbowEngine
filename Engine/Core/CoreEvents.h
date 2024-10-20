/**
 * @file CoreEvents.h
 * @author Echo 
 * @Date 24-6-11
 * @brief 
 */

#pragma once

#include "Event/Event.h"

DECLARE_EVENT(ProjectPathSetEvent, void);
DECLARE_EVENT(GetAppWindowSizeEvent, void, int*, int*);

DECLARE_MULTICAST_EVENT(AppExitEvent);
DECLARE_MULTICAST_EVENT(AppWindowResizeEvent, int32_t, int32_t);
DECLARE_MULTICAST_EVENT(AppNeedWaitEvent);
DECLARE_MULTICAST_EVENT(BackbufferResizeEvent, int32_t, int32_t);

inline ProjectPathSetEvent   OnProjectPathSet;
inline AppExitEvent          OnAppExit;
inline AppWindowResizeEvent  OnAppWindowResized;
inline AppNeedWaitEvent      OnAppNeedWait;
inline GetAppWindowSizeEvent OnGetAppWindowSize;
inline BackbufferResizeEvent OnBackbufferResize;
