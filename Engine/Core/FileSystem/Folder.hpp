//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/String/String.hpp"

class Folder {
public:
    Folder(StringView path) : path_(path) {}

    static bool IsFolderEmpty(StringView path);

    static Array<String> ListFiles(StringView path, bool recursive = false);

    static Array<String> ListFilesRegex(StringView path, StringView regex, bool recursive = false);

    static Array<String> ListFilesLambda(StringView path, const Function<bool(StringView)> &Func_Filter, bool recursive = false);

    static bool TryListFiles(StringView path, Array<String> &files, bool recursive = false);

    static bool TryListFilesRegex(StringView path, Array<String> &files, StringView regex, bool recursive = false);

    static bool TryListFilesLambda(StringView path, Array<String> &files, const Function<bool(StringView)> &Func_Filter, bool recursive = false);

    // clang-format on
    static bool ContainsFile(StringView path, StringView name, bool recursive = false);

    static bool CreateFolder(StringView path);

    [[nodiscard]] bool Create() const { return CreateFolder(path_); }
    [[nodiscard]] bool IsExist() const;

    [[nodiscard]] String GetAbsolutePath() const;
    [[nodiscard]] String GetRelativePath() const { return path_; }

private:
    String path_;
};
