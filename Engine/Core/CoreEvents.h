/**
 * @file CoreEvents.h
 * @author Echo 
 * @Date 24-6-11
 * @brief 
 */

#pragma once

#include "Core/Event/Event.h"


DECLARE_EVENT(RequireLoadFileTextEvent, core::Optional<core::String>, const core::String&);
DECLARE_EVENT(RequireWriteFileTextEvent, bool, const core::String&, const core::String&)

DECLARE_MULTICAST_EVENT(ProjectPathSetEvent, core::StringView)

inline RequireLoadFileTextEvent  Event_OnRequireReadFileText;
inline RequireWriteFileTextEvent Event_OnWriteFileText;
inline ProjectPathSetEvent       Event_OnProjectPathSet;
