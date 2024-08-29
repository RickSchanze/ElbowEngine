/**
 * @file TimeUtils.h
 * @author Echo 
 * @Date 24-8-30
 * @brief 
 */

#pragma once
#include "CoreDef.h"

class TimeUtils {
public:
    template <typename T>
    static AnsiString ToAnsiString(const std::chrono::time_point<T>& time_point, const std::string& format = "%Y-%m-%d %H:%M:%S")
    {
        std::time_t time = T::to_time_t(time_point);
        std::tm tm = *std::localtime(&time);

        std::ostringstream oss;
        oss << std::put_time(&tm, format.c_str());
        return oss.str();
    }
};
