/**
 * @file Folder.cpp
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#include "Folder.h"

#include "Path.h"

#include "Core/Base/EString.h"

#include <filesystem>
#include <regex>

bool platform::Folder::IsFolderEmpty(core::StringView path)
{
    if (!Path::IsExist(path))
    {
        return false;
    }
    if (!Path::IsFolder(path))
    {
        return false;
    }
    const auto path_view = path.GetStdStringView();
    const auto path1     = std::filesystem::path(path_view);
    const auto it        = std::filesystem::directory_iterator(path1);
    return it == std::filesystem::directory_iterator();
}

core::Array<core::String> platform::Folder::ListFiles(core::StringView path, bool recursive)
{
    core::Array<core::String> files;
    if (TryListFiles(path, files, recursive))
    {
        return files;
    }
    return {};
}

core::Array<core::String> platform::Folder::ListFilesRegex(core::StringView path, core::StringView regex, bool recursive)
{
    core::Array<core::String> files;
    if (TryListFilesRegex(path, files, regex, recursive))
    {
        return files;
    }
    return {};
}

core::Array<core::String>
platform::Folder::ListFilesLambda(core::StringView path, const core::Function<bool(core::StringView)>& Func_Filter, bool recursive)
{
    core::Array<core::String> files;
    if (TryListFiles(path, files, recursive))
    {
        return files;
    }
    return {};
}

static void ListFilesImpl(const std::filesystem::path& path, core::Array<core::String>& files, bool recursive)
{
    for (const auto& entry: std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            if (recursive) ListFilesImpl(entry.path(), files, recursive);
        }
        else
        {
            files.emplace_back(entry.path().filename().string());
        }
    }
}

bool platform::Folder::TryListFiles(core::StringView path, core::Array<core::String>& files, bool recursive)
{
    if (!Path::IsFolder(path))
    {
        return false;
    }
    ListFilesImpl(path.GetStdStringView(), files, recursive);
    return true;
}

static void ListFilesImplRegex(const std::filesystem::path& path, core::Array<core::String>& files, core::StringView regex, bool recursive)
{
    for (const auto& entry: std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            if (recursive) ListFilesImplRegex(entry.path(), files, regex, recursive);
        }
        else
        {
            if (std::regex_match(entry.path().filename().string(), std::regex(regex.Data())))
            {
                files.emplace_back(entry.path().filename().string());
            }
        }
    }
}

bool platform::Folder::TryListFilesRegex(core::StringView path, core::Array<core::String>& files, core::StringView regex, bool recursive)
{
    if (!Path::IsFolder(path))
    {
        return false;
    }
    ListFilesImplRegex(path.GetStdStringView(), files, regex, recursive);
    return true;
}

static void ListFilesImplLambda(
    const std::filesystem::path& path, core::Array<core::String>& files, const core::Function<bool(core::StringView)>& Func_Filter, bool recursive
)
{
    for (const auto& entry: std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            if (recursive) ListFilesImplLambda(entry.path(), files, Func_Filter, recursive);
        }
        else
        {
            if (Func_Filter(entry.path().filename().string()))
            {
                files.emplace_back(entry.path().filename().string());
            }
        }
    }
}

bool platform::Folder::TryListFilesLambda(
    core::StringView path, core::Array<core::String>& files, const core::Function<bool(core::StringView)>& Func_Filter, bool recursive
)
{
    if (!Path::IsFolder(path))
    {
        return false;
    }
    ListFilesImplLambda(path.GetStdStringView(), files, Func_Filter, recursive);
    return true;
}

static bool FindFileImpl(const std::filesystem::path& path, core::StringView name, bool recursive, core::String& found)
{
    for (const auto& entry: std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            if (recursive) FindFileImpl(entry.path(), name, recursive, found);
        }
        else
        {
            if (entry.path().filename().string() == name.GetStdStringView())
            {
                found = entry.path().string();
                return true;
            }
        }
    }
    return false;
}

bool platform::Folder::ContainsFile(core::StringView path, core::StringView name, bool recursive)
{
    core::String found;
    return FindFileImpl(path.GetStdStringView(), name, recursive, found);
}

bool platform::Folder::CreateFolder(core::StringView path)
{
    auto v = path.GetStdStringView();
    return std::filesystem::create_directory(v);
}

bool platform::Folder::Create() const
{
    return CreateFolder(path_);
}

bool platform::Folder::IsExist() const
{
    return Path::IsExist(GetAbsolutePath());
}

core::String platform::Folder::GetAbsolutePath() const
{
    return Path::Combine(Path::GetProjectPath(), path_);
}

core::String platform::Folder::GetRelativePath() const
{
    return path_;
}
