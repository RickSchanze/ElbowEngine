/**
 * @file Logger.h
 * @author Echo 
 * @Date 24-4-6
 * @brief 
 */
#pragma once

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include "Exception/Exception.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
class Logger
{
public:
    Logger();


    /** 输出Info级别的信息 */
    template<typename... ArgsT>
    void Info(spdlog::wformat_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->info(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Info级别的信息 */
    template<typename... ArgsT>
    void Info(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->info(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Debug级别的信息 */
    template<typename... ArgsT>
    void Debug(spdlog::wformat_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->debug(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Debug级别的信息 */
    template<typename... ArgsT>
    void Debug(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->debug(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Error级别的信息 */
    template<typename... ArgsT>
    void Error(spdlog::wformat_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->error(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Error级别的信息 */
    template<typename... ArgsT>
    void Error(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->error(fmt, Forward<ArgsT>(args)...);
    }


    /** 输出Warning级别的信息 */
    template<typename... ArgsT>
    void Warning(spdlog::wformat_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->warn(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Warning级别的信息 */
    template<typename... ArgsT>
    void Warning(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->warn(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Trace级别的信息 */
    template<typename... ArgsT>
    void Trace(spdlog::wformat_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->trace(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Trace级别的信息 */
    template<typename... ArgsT>
    void Trace(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->trace(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Critical级别的信息 会自动Crash程序 */
    template<typename... ArgsT>
    void Critical(spdlog::wformat_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        GENERATE_STACKTRACE()
        logger_->critical(fmt, Forward<ArgsT>(args)...);
        logger_->critical("{}", CurrentStackTraceStr);
#ifdef ELBOW_DEBUG
        __debugbreak();
#else
        assert((false, "trigger critical log, crash program"));
#endif
        exit(-1);
    }

    /** 输出Critical级别的信息 会自动Crash程序 */
    template<typename... ArgsT>
    void Critical(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        GENERATE_STACKTRACE()
        logger_->critical(fmt, Forward<ArgsT>(args)...);
        logger_->critical("{}", CurrentStackTraceStr);
#ifdef ELBOW_DEBUG
        __debugbreak();
#else
        assert((false, "trigger critical log, crash program"));
#endif
        exit(-1);
    }

    /**
     * 输出一个Exception
     * @param ex
     */
    void Exception(const Exception& ex) const
    {
        logger_->error(L"未处理的异常:\n {}", ex.What());
        logger_->error("{}", ex.GetStackTrace());
    }

    /**
     * 输出一个std::exception
     * @param ex 输出的错误
     */
    void StdException(const std::exception& ex) const
    {
        logger_->error("Unhandled exception:\n {}", ex.what());
    }

private:
    TSharedPtr<spdlog::logger> logger_;
};
