/**
 * @file File.cpp
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#include "File.h"
#include "Path.h"
#include <filesystem>

core::String platform::File::GetFolder() const
{
    return std::filesystem::path(path_.StdString()).parent_path().string();
}

core::String platform::File::GetFileName() const
{
    return std::filesystem::path(path_.StdString()).filename().string();
}

core::String platform::File::GetFileNameWithoutExtension() const
{
    return std::filesystem::path(path_.StdString()).stem().string();
}

core::String platform::File::GetAbsolutePath() const
{
    return Path::GetProjectPath() + "/" + path_;
}

core::String platform::File::GetRelativePath() const
{
    return path_;
}

bool platform::File::IsExist() const
{
    const auto s = GetAbsolutePath();
    return Path::IsExist(s);
}
