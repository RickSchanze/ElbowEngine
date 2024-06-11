/**
 * @file CoreEvents.h
 * @author Echo 
 * @Date 24-6-11
 * @brief 
 */

#pragma once

#include "Event/Event.h"

struct ProjectPathSetEvent : TEvent<>
{

};

inline ProjectPathSetEvent OnProjectPathSet;
