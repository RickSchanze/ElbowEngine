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
class Logger {
public:
    Logger() {
        mLogger = spdlog::stdout_color_mt("ElbowEngine");
        mLogger->set_pattern("[%Y-%m-%d %H:%M:%S] [thread: %t] [%l] %v");
    }

    /** 输出Info级别的信息 */
    template<typename... ArgsT>
    void Info(spdlog::wformat_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->info(Fmt, Forward<ArgsT>(Args)...);
    }

    /** 输出Info级别的信息 */
    template<typename... ArgsT>
    void Info(spdlog::format_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->info(Fmt, Forward<ArgsT>(Args)...);
    }

    /** 输出Debug级别的信息 */
    template<typename... ArgsT>
    void Debug(spdlog::wformat_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->debug(Fmt, Forward<ArgsT>(Args)...);
    }

    /** 输出Debug级别的信息 */
    template<typename... ArgsT>
    void Debug(spdlog::format_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->debug(Fmt, Forward<ArgsT>(Args)...);
    }

    /** 输出Error级别的信息 */
    template<typename... ArgsT>
    void Error(spdlog::wformat_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->error(Fmt, Forward<ArgsT>(Args)...);
    }

    /** 输出Error级别的信息 */
    template<typename... ArgsT>
    void Error(spdlog::format_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->error(Fmt, Forward<ArgsT>(Args)...);
    }


    /** 输出Warning级别的信息 */
    template<typename... ArgsT>
    void Warning(spdlog::wformat_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->warn(Fmt, Forward<ArgsT>(Args)...);
    }

    /** 输出Warning级别的信息 */
    template<typename... ArgsT>
    void Warning(spdlog::format_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->warn(Fmt, Forward<ArgsT>(Args)...);
    }

    /** 输出Trace级别的信息 */
    template<typename... ArgsT>
    void Trace(spdlog::wformat_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->trace(Fmt, Forward<ArgsT>(Args)...);
    }

    /** 输出Trace级别的信息 */
    template<typename... ArgsT>
    void Trace(spdlog::format_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        mLogger->trace(Fmt, Forward<ArgsT>(Args)...);
    }

    /** 输出Critical级别的信息 会自动Crash程序 */
    template<typename... ArgsT>
    void Critical(spdlog::wformat_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        GENERATE_STACKTRACE()
        mLogger->critical(Fmt, Forward<ArgsT>(Args)...);
        mLogger->critical("{}", CurrentStackTraceStr);
        exit(-1);
    }

    /** 输出Critical级别的信息 会自动Crash程序 */
    template<typename... ArgsT>
    void Critical(spdlog::format_string_t<ArgsT...> Fmt, ArgsT&&... Args) {
        GENERATE_STACKTRACE()
        mLogger->critical(Fmt, Forward<ArgsT>(Args)...);
        mLogger->critical("{}", CurrentStackTraceStr);
        exit(-1);
    }

    /**
     * 输出一个Exception
     * @param Exception
     */
    void Exception(const Exception& Exception) const {
        mLogger->error(L"未处理的异常:\n {}", Exception.What());
        mLogger->error("{}", Exception.GetStackTrace());
    }

    /**
     * 输出一个std::exception
     * @param Exception 输出的错误
     */
    void StdException(const std::exception& Exception) const {
        mLogger->error("Unhandled exception:\n {}", Exception.what());
    }

private:
    SharedPtr<spdlog::logger> mLogger;
};

#define INFO Logger::Get().Info
