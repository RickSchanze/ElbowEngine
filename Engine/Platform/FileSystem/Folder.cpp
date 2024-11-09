/**
 * @file Folder.cpp
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#include "Folder.h"

#include "Path.h"

#include "Base/EString.h"

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
    const auto path_view = path.ToStdStringView();
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

core::Array<core::String> platform::Folder::ListFiles(core::StringView path, core::StringView regex, bool recursive)
{
    core::Array<core::String> files;
    if (TryListFiles(path, files, regex, recursive))
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
            files.push_back(entry.path().filename().string());
        }
    }
}

bool platform::Folder::TryListFiles(core::StringView path, core::Array<core::String>& files, bool recursive)
{
    if (!Path::IsFolder(path))
    {
        return false;
    }
    ListFilesImpl(path.ToStdStringView(), files, recursive);
    return true;
}

static void ListFilesImpl(const std::filesystem::path& path, core::Array<core::String>& files, core::StringView regex, bool recursive)
{
    for (const auto& entry: std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            if (recursive) ListFilesImpl(entry.path(), files, regex, recursive);
        }
        else
        {
            if (std::regex_match(entry.path().filename().string(), std::regex(regex.ToStdStringView())))
            {
                files.push_back(entry.path().filename().string());
            }
        }
    }
}

bool platform::Folder::TryListFiles(core::StringView path, core::Array<core::String>& files, core::StringView regex, bool recursive)
{
    if (!Path::IsFolder(path))
    {
        return false;
    }
    ListFilesImpl(path.ToStdStringView(), files, regex, recursive);
    return true;
}

static void FindFileImpl(const std::filesystem::path& path, core::StringView name, bool recursive, core::String& found)
{
    for (const auto& entry: std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            if (recursive) FindFileImpl(entry.path(), name, recursive, found);
        }
        else
        {
            if (entry.path().filename().string() == name.ToStdStringView())
            {
                found = entry.path().string();
                return;
            }
        }
    }
}

bool platform::Folder::ContainsFile(core::StringView path, core::StringView name, bool recursive)
{
    core::String found;
    return FindFileImpl(path.ToStdStringView(), name, recursive, found);
}

void platform::Folder::CreateFolder() const
{
    if
}
