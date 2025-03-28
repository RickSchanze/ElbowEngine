//
// Created by Echo on 2025/3/25.
//
#include "Folder.hpp"

#include <filesystem>
#include <regex>

#include "Core/Logger/Logger.hpp"
#include "Core/Misc/Other.hpp"
#include "Core/Profile.hpp"
#include "Path.hpp"

static void ListFilesImpl(const std::filesystem::path &path, Array<String> &files, const bool recursive) {
    ProfileScope _(__func__);
    for (const auto &entry: std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            if (recursive)
                ListFilesImpl(entry.path(), files, recursive);
        } else {
            files.Emplace(entry.path().filename().string());
        }
    }
}

static void ListFilesImplRegex(const std::filesystem::path &path, Array<String> &files, StringView regex, const bool recursive) {
    ProfileScope _(__func__);
    for (const auto &entry: std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            if (recursive)
                ListFilesImplRegex(entry.path(), files, regex, recursive);
        } else {
            if (std::regex_match(entry.path().filename().string(), std::regex(regex.Data()))) {
                files.Emplace(entry.path().filename().string());
            }
        }
    }
}

static void ListFilesImplLambda(const std::filesystem::path &path, Array<String> &files, const Function<bool(StringView)> &Func_Filter,
                                const bool recursive) {
    ProfileScope _(__func__);
    for (const auto &entry: std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            if (recursive)
                ListFilesImplLambda(entry.path(), files, Func_Filter, recursive);
        } else {
            if (Func_Filter(entry.path().filename().string())) {
                files.Emplace(entry.path().filename().string());
            }
        }
    }
}

static bool FindFileImpl(const std::filesystem::path &path, StringView name, const bool recursive, String &found) {
    ProfileScope _(__func__);
    for (const auto &entry: std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            if (recursive)
                FindFileImpl(entry.path(), name, recursive, found);
        } else {
            if (entry.path().filename().string() == name.ToStdStringView()) {
                found = entry.path().string();
                return true;
            }
        }
    }
    return false;
}

bool Folder::IsFolderEmpty(StringView path) {
    ProfileScope _(__func__);
    if (!Path::IsExist(path)) {
        return false;
    }
    if (!Path::IsFolder(path)) {
        return false;
    }
    const auto path_view = path.ToStdStringView();
    const auto path1 = std::filesystem::path(path_view);
    const auto it = std::filesystem::directory_iterator(path1);
    return it == std::filesystem::directory_iterator();
}

Array<String> Folder::ListFiles(StringView path, bool recursive) {
    ProfileScope _(__func__);
    Array<String> files;
    if (TryListFiles(path, files, recursive)) {
        return files;
    }
    return {};
}

Array<String> Folder::ListFilesRegex(StringView path, StringView regex, bool recursive) {
    ProfileScope _(__func__);
    Array<String> files;
    if (TryListFilesRegex(path, files, regex, recursive)) {
        return files;
    }
    return {};
}

Array<String> Folder::ListFilesLambda(StringView path, const Function<bool(StringView)> &Func_Filter, bool recursive) {
    ProfileScope _(__func__);
    Array<String> files;
    if (TryListFiles(path, files, recursive)) {
        return files;
    }
    return {};
}

bool Folder::TryListFiles(StringView path, Array<String> &files, const bool recursive) {
    ProfileScope _(__func__);
    if (!Path::IsFolder(path)) {
        return false;
    }
    ListFilesImpl(path.ToStdStringView(), files, recursive);
    return true;
}

bool Folder::TryListFilesRegex(StringView path, Array<String> &files, StringView regex, const bool recursive) {
    ProfileScope _(__func__);
    if (!Path::IsFolder(path)) {
        return false;
    }
    ListFilesImplRegex(path.ToStdStringView(), files, regex, recursive);
    return true;
}

bool Folder::TryListFilesLambda(StringView path, Array<String> &files, const Function<bool(StringView)> &Func_Filter, const bool recursive) {
    ProfileScope _(__func__);
    if (!Path::IsFolder(path)) {
        return false;
    }
    ListFilesImplLambda(path.ToStdStringView(), files, Func_Filter, recursive);
    return true;
}

bool Folder::ContainsFile(StringView path, StringView name, const bool recursive) {
    String found;
    return FindFileImpl(path.ToStdStringView(), name, recursive, found);
}

bool Folder::CreateFolder(StringView path) {
    auto v = path.ToStdStringView();
    try {
        return std::filesystem::create_directories(v);
    } catch (std::exception &e) {
        Log(Fatal) << "Exception: "_es + e.what();
        Exit(1);
        return false;
    }
}

bool Folder::IsExist() const { return Path::IsExist(GetAbsolutePath()); }

String Folder::GetAbsolutePath() const { return Path::Combine(Path::GetProjectPath(), path_); }
