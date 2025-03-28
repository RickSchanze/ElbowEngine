//
// Created by Echo on 2025/3/25.
//
#include "File.hpp"

#include <filesystem>
#include <fstream>

#include "Core/Logger/Logger.hpp"
#include "Folder.hpp"
#include "Path.hpp"

bool File::IsExist() const {
    const auto s = GetAbsolutePath();
    return Path::IsExist(s);
}

bool File::Create_File(StringView path, FileCreateMode mode, const bool create_folder, const bool overwrite) {
    if (Path::IsExist(path)) {
        if (!overwrite) {
            Log(Error) << "File already exists: "_es + path;
            return false;
        }
    }
    auto parent_path = Path::GetParent(path);
    if (!Path::IsExist(parent_path)) {
        if (!create_folder) {
            Log(Error) << "Parent folder not exist: "_es + parent_path;
            return false;
        }
        if (!Folder::CreateFolder(parent_path)) {
            Log(Error) << "Failed to create parent folder: "_es + parent_path;
            return false;
        }
    }
    if (mode == FileCreateMode::Text) {
        if (const std::ofstream file(path.Data(), std::ios::binary); !file) {
            return false;
        }
    } else if (mode == FileCreateMode::Binary) {
        if (const std::ofstream file(path.Data(), std::ios::binary); !file) {
            return false;
        }
    }
    return true;
}

String File::GetAbsolutePath() const {
    // 看看是不是已经是绝对路径了
    std::filesystem::path p = path_.ToWideString();
    if (p.is_absolute())
        return path_;
    return Path::GetProjectPath() + "/" + path_;
}

bool File::TryReadAllText(String &out) const {
    if (!IsExist()) {
        Log(Error) << "File not exist: "_es + path_;
        return false;
    }
    std::ifstream file(*path_, std::ios::in);
    if (!file.is_open()) {
        Log(Error) << "Failed to open file: "_es + path_;
        return false;
    }
    out.Clear();
    out = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return true;
}

bool File::WriteAllText(StringView text) const {
    if (Create()) {
        std::ofstream file(path_.Data(), std::ios::out);
        if (!file.is_open()) {
            Log(Error) << "Failed to open file: "_es + path_;
            return false;
        }
        file << *text;
        return true;
    } else {
        Log(Error) << "Failed to create file: "_es + path_;
        return false;
    }
}
