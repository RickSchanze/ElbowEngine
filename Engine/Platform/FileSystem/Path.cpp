/**
 * @file Path.cpp
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#include "Path.h"

#include "Core/CoreDef.h"
#include "Core/CoreEvents.h"
#include "Core/Log/Logger.h"
#include "File.h"
#include "Folder.h"
#include "Platform/PlatformLogcat.h"

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
    core::Assert::Require(logcat::Platform_FileSystem, IsProjectPathValid(s_proj_path_), "Project path is not valid!");
    return s_proj_path_;
}

bool platform::Path::SetProjectPath(core::StringView path)
{
    if (path.IsEmpty() || path.IsPureSpace())
    {
        LOGGER.Error(logcat::Platform_FileSystem, "Project path is empty or pure space!");
        return false;
    }
    if (!IsProjectPathValid(path))
    {
        LOGGER.Error(logcat::Platform_FileSystem, "The project path must either be an empty folder or contain an .elbowengine file.");
        return false;
    }
    s_proj_path_ = path;
    std::filesystem::current_path(s_proj_path_.Data());
    Event_OnProjectPathSet.Invoke(s_proj_path_);
    return true;
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

core::String platform::Path::GetParent(core::StringView path)
{
    auto parent = std::filesystem::path(path.GetStdStringView()).parent_path().string();
    if (parent.empty())
    {
        return GetProjectPath();
    }
    return parent;
}

core::StringView platform::Path::GetFileName(const core::StringView file_path)
{
    const auto   p              = file_path.GetStdStringView();
    const size_t last_slash_pos = p.find_last_of("\\/");
    const size_t start          = last_slash_pos == std::string::npos ? 0 : last_slash_pos + 1;
    return p.substr(start);
}

core::StringView platform::Path::GetFileNameWithoutExt(const core::StringView file_path)
{
    const auto p   = GetFileName(file_path).GetStdStringView();
    const auto pos = p.find_last_of('.');
    if (pos == std::string::npos) return p;
    return p.substr(0, pos);
}
