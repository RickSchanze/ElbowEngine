/**
 * @file File.cpp
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#include "File.h"
#include "Core/Log/Logger.h"
#include "Path.h"
#include "Platform/PlatformLogcat.h"
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
    return std::filesystem::path(path_.GetStdString()).parent_path().string();
}

core::String platform::File::GetFileName() const
{
    return std::filesystem::path(path_.GetStdString()).filename().string();
}

core::String platform::File::GetFileNameWithoutExtension() const
{
    return std::filesystem::path(path_.GetStdString()).stem().string();
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

bool platform::File::TryReadAllText(core::String& out, bool combine_proj_path) const
{
    core::String path = combine_proj_path ? GetAbsolutePath() : path_;
    if (!IsExist())
    {
        LOGGER.Error(logcat::Platform_FileSystem, "File not exist: {}", path);
        return false;
    }
    std::ifstream file(path, std::ios::in);
    if (!file.is_open())
    {
        LOGGER.Error(logcat::Platform_FileSystem, "Failed to open file: {}", path);
        return false;
    }
    out.Clear();
    out = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return true;
}

Optional<String> platform::File::ReadAllText(bool combine) const
{
    String     text;
    const auto err = TryReadAllText(text, combine);
    return err ? std::make_optional(text) : NullOpt;
}

bool platform::File::Create(FileCreateMode mode, bool combine_proj_path) const
{
    return Create_File(path_, mode, combine_proj_path);
}
