/**
 * @file PlatformDefines.h
 * @author Echo 
 * @Date 24-11-22
 * @brief 
 */

#pragma once
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__linux__)
    #define PLATFORM_LINUX
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define PLATFORM_APPLE
    #endif
    #if TARGET_OS_IPHONE
        #define PLATFORM_IOS
    #endif
#elif defined(__ANDROID__)
    #define PLATFORM_ANDROID
#else
    #error "Unknown platform"
#endif
