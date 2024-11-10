/**
 * @file Path.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "Base/CoreTypeDef.h"
#include "Base/EString.h"
#include "File.h"

namespace platform
{
class Path
{
public:
    static core::String     Combine(core::StringView left, core::StringView right);
    static core::StringView GetProjectPath();
    static bool             SetProjectPath(core::StringView path);
    static bool             HasInvalidCharacter(core::StringView path);
    static bool             IsExist(core::StringView path);
    static bool             IsFolder(core::StringView path);

private:
    static inline core::String s_proj_path_;
};
}   // namespace platform
