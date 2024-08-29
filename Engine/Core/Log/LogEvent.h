/**
 * @file LogEvent.h
 * @author Echo 
 * @Date 24-8-28
 * @brief 
 */

#pragma once
#include "Event/Event.h"

struct LogEvent : TEvent<const spdlog::details::log_msg&> {};

inline LogEvent OnLog;
