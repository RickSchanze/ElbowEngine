/**
 * @file LogEvent.h
 * @author Echo 
 * @Date 24-8-28
 * @brief 
 */

#pragma once
#include "Event/Event.h"

DECLARE_MULTICAST_EVENT(LogEvent, const spdlog::details::log_msg&);

inline LogEvent OnLog;
