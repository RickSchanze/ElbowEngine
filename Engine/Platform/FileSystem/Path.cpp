/**
 * @file Path.cpp
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#include "Path.h"

#include "CoreDef.h"
#include "File.h"
#include "Folder.h"
#include "Log/Logger.h"
#include "PlatformLogcat.h"

#include <filesystem>
static bool IsProjectPathValid(core::StringView path)
{
    // path必须是一个文件夹
    if (!platform::Path::IsFolder(path)) return false;
    // 文件夹要么为空, 要么必须有.elbowengine文件
    if (platform::Folder::IsFolderEmpty(path))
    {
        return true;
    }
    if (platform::Folder::ContainsFile(path, ".elbowengine"))
    {
        return true;
    }
    return false;
}

core::String platform::Path::Combine(core::StringView left, core::StringView right)
{
    if (left.EndsWith("/"))
    {
        return left + right;
    }
    return left + "/" + right;
}

core::StringView platform::Path::GetProjectPath()
{
    Assert(logcat::Platform_FileSystem, IsProjectPathValid(s_proj_path_), "Project path is not valid!");
    return s_proj_path_;
}

bool platform::Path::SetProjectPath(core::StringView path)
{
    if (path.IsEmpty() || path.IsPureSpace())
    {
        LOGGER.Error(logcat::Platform_FileSystem, "Project path is empty or pure space!");
        return false;
    }
    if (HasInvalidCharacter(path))
    {
        LOGGER.Error(logcat::Platform_FileSystem, "Project path contains invalid characters!");
        return false;
    }
    if (!IsProjectPathValid(path))
    {
        LOGGER.Error(logcat::Platform_FileSystem, "The project path must either be an empty folder or contain an .elbowengine file.");
        return false;
    }
    s_proj_path_ = path;
    return true;
}

bool platform::Path::HasInvalidCharacter(core::StringView path)
{
    // 此函数用于找出path里面有没不能用于路径的非法字符
    // 常见的文件路径非法字符
    const core::StringView invalidChars =
        // 基本文件系统非法字符
        "<>:\"|?*"
        // NULL字符和控制字符
        "\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17"
        "\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37"
        // 其他可能有问题的字符
        "\\/"   // 在需要的地方可以单独处理路径分隔符
        // Unicode 方向控制字符
        "\u202A\u202B\u202C\u202D\u202E"   // LRE, RLE, PDF, LRO, RLO
        "\u2066\u2067\u2068\u2069"         // LRI, RLI, FSI, PDI
        // Zero-width characters
        "\u200B\u200C\u200D\u200E\u200F"   // ZWSP, ZWNJ, ZWJ, LRM, RLM
        // 其他潜在危险的Unicode字符
        "\uFEFF";   // Byte Order Mark
    return !path.ContainsAny(invalidChars, true);
}

bool platform::Path::IsExist(core::StringView path)
{
    return std::filesystem::exists(path.GetStdStringView());
}

bool platform::Path::IsFolder(core::StringView path)
{
    if (!IsExist(path)) return false;
    return std::filesystem::is_directory(path.GetStdStringView());
}
