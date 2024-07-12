/**
 * @file Logger.cpp
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */

#include "Logger.h"

Logger::Logger()
{
    logger_ = spdlog::stdout_color_mt("ElbowEngine");
    logger_->set_pattern("[%Y-%m-%d %H:%M:%S] [thread: %t] [%l] %v");
}
