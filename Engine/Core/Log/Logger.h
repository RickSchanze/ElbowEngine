/**
 * @file Logger.h
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */

#ifndef LOGGER_H
#define LOGGER_H
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include "Core/CoreDef.h"
#include "spdlog/spdlog.h"


class Logger {
public:
    Logger() {
        spdlog::log(spdlog::level::info, String(L"你好"));
    }
};



#endif //LOGGER_H
