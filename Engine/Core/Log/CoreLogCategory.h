/**
 * @file CoreLogCategory.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once

namespace logcat
{
constexpr auto Unknown                 = "Unknown";
constexpr auto Engine                  = "Engine";
constexpr auto Async                   = "Async";
constexpr auto Async_Coro              = "Async.Coro";
constexpr auto StackTrace              = "StackTrace";
constexpr auto Test                    = "Test";
constexpr auto Archive                 = "Archive";
constexpr auto Archive_Serialization   = "Archive.Serialization";
constexpr auto Archive_Deserialization = "Archive.Deserialization";
constexpr auto Reflection              = "Reflection";
constexpr auto Core                    = "Core";
constexpr auto Core_Object             = "Core.Object";
constexpr auto Core_Event              = "Core.Event";
constexpr auto Config                  = "Config";
constexpr auto Core_Memory             = "Core.Memory";
constexpr auto Exception               = "Exception";
constexpr auto Core_Thread             = "Core.Thread";
}   // namespace logcat
