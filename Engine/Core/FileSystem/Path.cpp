//
// Created by Echo on 2025/3/21.
//
#include "Path.hpp"

#include <filesystem>
#include <string>

#include "Core/Logger/Logger.hpp"
#include "Core/String/String.hpp"
#include "Folder.hpp"


bool IsProjectPathValid(StringView path) {
    // path必须是一个文件夹
    if (!Path::IsFolder(path))
        return false;
    // 文件夹要么为空, 要么必须有.elbowengine文件
    if (Folder::IsFolderEmpty(path)) {
        return true;
    }
    if (Folder::ContainsFile(path, ".elbowengine")) {
        return true;
    }
    return false;
}

String Path::Combine(StringView left, StringView right) {
    if (left.EndsWith("/")) {
        return left + right;
    }
    return left + "/" + right;
}

bool Path::SetProjectPath(StringView path) {
    if (path.IsEmpty() || path.IsPureSpace()) {
        Log(Error) << "The project path is empty or pure space!";
        return false;
    }
    if (!IsProjectPathValid(path)) {
        Log(Error) << "The project path must either be an empty folder or contain an .elbowengine file.";
        return false;
    }
    s_proj_path_ = path;
    std::filesystem::current_path(s_proj_path_.Data());
    Evt_OnProjectPathSet.Invoke(s_proj_path_);
    return true;
}

bool Path::IsFolder(StringView path) {
    if (!IsExist(path))
        return false;
    return std::filesystem::is_directory(path.ToStdStringView());
}

String Path::GetParent(StringView path) {
    auto parent = std::filesystem::path(path.ToStdStringView()).parent_path().string();
    if (parent.empty()) {
        return GetProjectPath();
    }
    return parent;
}

StringView Path::GetFileName(StringView file_path) {
    const auto p = file_path.ToStdStringView();
    const size_t last_slash_pos = p.find_last_of("\\/");
    const size_t start = last_slash_pos == std::string::npos ? 0 : last_slash_pos + 1;
    return p.substr(start);
}

StringView Path::GetFileNameWithoutExt(StringView file_path) {
    const auto p = GetFileName(file_path).ToStdStringView();
    const auto pos = p.find_last_of('.');
    if (pos == std::string::npos)
        return p;
    return p.substr(0, pos);
}

String Path::GetAbsolutePath(StringView view) {
    if (view.StartsWith(s_proj_path_)) {
        return view;
    }
    return s_proj_path_ + "/" + view;
}
