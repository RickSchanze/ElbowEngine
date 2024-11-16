/**
 * @file LogEvent.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once
#include "Core/Event/Event.h"

namespace core
{
struct Log;
}

DECLARE_MULTICAST_EVENT(LogEvent, core::Log&);
inline LogEvent Event_OnLog;
