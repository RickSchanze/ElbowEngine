/**
 * @file TimeUtils.cpp
 * @author Echo 
 * @Date 24-8-30
 * @brief 
 */

#include "TimeUtils.h"

#include <algorithm>

Millisecond TimeUtils::Clamp(Millisecond time, Millisecond min, Millisecond max)
{
    return std::clamp(time, min, max);
}