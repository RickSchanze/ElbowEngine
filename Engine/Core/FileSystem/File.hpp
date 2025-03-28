//
// Created by Echo on 2025/3/21.
//
#pragma once
#include <filesystem>
#include "Core/Misc/Optional.hpp"
#include "Core/String/String.hpp"

enum class FileCreateMode { Text, Binary };

class File {
public:
    File() = default;
    File(const String &path) : path_(path) {}

    static bool Create_File(StringView path, FileCreateMode mode = FileCreateMode::Text, bool create_folder = true, bool overwrite = true);

    [[nodiscard]] String GetFolder() const { return std::filesystem::path(path_.ToStdString()).parent_path().string(); }

    [[nodiscard]] String GetFileName() const { return std::filesystem::path(path_.ToStdString()).filename().string(); }

    [[nodiscard]] String GetFileNameWithoutExtension() const { return std::filesystem::path(path_.ToStdString()).stem().string(); }

    [[nodiscard]] String GetAbsolutePath() const;
    [[nodiscard]] String GetRelativePath() const { return path_; }

    [[nodiscard]] bool IsExist() const;

    static bool IsExist(StringView path) { return File(path).IsExist(); }

    bool TryReadAllText(String &out) const;

    [[nodiscard]] Optional<String> ReadAllText() const {
        if (String text; TryReadAllText(text)) {
            return MakeOptional(text);
        }
        return {};
    }

    static Optional<String> ReadAllText(const String &path) {
        const File file = path;
        return file.ReadAllText();
    }
    [[nodiscard]] bool WriteAllText(StringView text) const;

    static bool WriteAllText(const String &path, StringView text) {
        const File file = path;
        return file.WriteAllText(text);
    }

    [[nodiscard]] bool Create(const FileCreateMode mode = FileCreateMode::Text, const bool create_folder = true, const bool overwrite = true) const {
        return Create_File(path_, mode, create_folder, overwrite);
    }

    bool operator==(const File &f) const { return path_ == f.path_; }

private:
    // 相对于项目工程路径的路径
    String path_;
};
