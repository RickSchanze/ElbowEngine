/**
 * @file MathTypes.cpp
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#include "MathTypes.h"

#include <format>
String Size2D::ToString() {
    return std::format(L"Width: {}, Height:{}", Width, Height);
}
