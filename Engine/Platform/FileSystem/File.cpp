/**
 * @file File.cpp
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#include "File.h"
#include "Log/Logger.h"
#include "Path.h"
#include "PlatformLogcat.h"
#include <filesystem>
#include <fstream>

using namespace core;

bool platform::File::Create_File(StringView path, FileCreateMode mode, bool combine_proj_path)
{
    String full_path = combine_proj_path ? String(Path::GetProjectPath() + "/" + path) : String(path);
    if (mode == FileCreateMode::Text)
    {
        if (const std::ofstream file(full_path, std::ios::binary); !file)
        {
            return false;
        }
    }
    else if (mode == FileCreateMode::Binary)
    {
        if (const std::ofstream file(full_path, std::ios::binary); !file)
        {
            return false;
        }
    }
    return true;
}

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

platform::FileSystemError platform::File::TryReadAllText(core::String& out, bool combine_proj_path) const
{
    core::String path = combine_proj_path ? GetAbsolutePath() : path_;
    if (!IsExist())
    {
        LOGGER.Error(logcat::Platform_FileSystem, "File not exist: {}", path);
        return FileSystemError::FileNotFound;
    }
    std::ifstream file(path, std::ios::in);
    if (!file.is_open())
    {
        LOGGER.Error(logcat::Platform_FileSystem, "Failed to open file: {}", path);
        return FileSystemError::FailedToOpenFile;
    }
    out.Clear();
    file.seekg(0, std::ios::end);
    out.Reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&out[0], file.tellg());
    return FileSystemError::Success;
}

String platform::File::ReadAllText(bool combine) const
{
    String text;
    auto   err = TryReadAllText(text, combine);
    if (err != FileSystemError::Success)
    {
        return "Error";
    }
    return text;
}

bool platform::File::Create(FileCreateMode mode, bool combine_proj_path) const
{
    return Create_File(path_, mode, combine_proj_path);
}