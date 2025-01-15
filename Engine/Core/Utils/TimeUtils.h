/**
 * @file TimeUtils.h
 * @author Echo 
 * @Date 24-8-30
 * @brief 
 */

#pragma once
#include "Core/CoreDef.h"
#include <chrono>
using Millisecond = std::chrono::duration<float, std::milli>;

class TimeUtils {
public:
    static Millisecond Clamp(Millisecond time, Millisecond min, Millisecond max);
    static Float ToSeconds(Millisecond time);
};
