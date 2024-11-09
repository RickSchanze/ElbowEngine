/**
 * @file CoreLogCategory.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once

namespace logcat
{
constexpr auto Unknown               = "Unknown";
constexpr auto Engine                = "Engine";
constexpr auto Async                 = "Async";
constexpr auto Async_Coro            = "Async.Coro";
constexpr auto StackTrace            = "StackTrace";
constexpr auto Test                  = "Test";
constexpr auto Archive               = "Archive";
constexpr auto Archive_Serialization = "Archive.Serialization";
constexpr auto Reflection            = "Reflection";
}   // namespace logcat
