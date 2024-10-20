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
    void InfoAnsi(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
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
    void DebugAnsi(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->debug(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Error级别的信息 */
    template<typename... ArgsT>
    void Error(spdlog::wformat_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->error(fmt, Forward<ArgsT>(args)...);
        __debugbreak();
    }

    /** 输出Error级别的信息 */
    template<typename... ArgsT>
    void ErrorAnsi(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->error(fmt, Forward<ArgsT>(args)...);
        __debugbreak();
    }

    /** 输出Error级别的信息 */
    template<typename... ArgsT>
    void ErrorNoBreak(spdlog::wformat_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        logger_->error(fmt, Forward<ArgsT>(args)...);
    }

    /** 输出Error级别的信息 */
    template<typename... ArgsT>
    void ErrorNoBreakAnsi(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
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
    void WarningAnsi(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
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
    void TraceAnsi(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
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
#if ELBOW_DEBUG
        __debugbreak();
#else
        assert((false, "trigger critical log, crash program"));
#endif
        exit(-1);
    }

    /** 输出Critical级别的信息 会自动Crash程序 */
    template<typename... ArgsT>
    void CriticalAnsi(spdlog::format_string_t<ArgsT...> fmt, ArgsT&&... args)
    {
        GENERATE_STACKTRACE()
        logger_->critical(fmt, Forward<ArgsT>(args)...);
        logger_->critical("{}", CurrentStackTraceStr);
#if ELBOW_DEBUG
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
        __debugbreak();
    }

    /**
     * 输出一个std::exception
     * @param ex 输出的错误
     */
    void StdException(const std::exception& ex) const
    {
        logger_->error("Unhandled exception:\n {}", ex.what());
        __debugbreak();
    }

private:
    SharedPtr<spdlog::logger> logger_;
};
