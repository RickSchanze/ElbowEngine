/**
 * @file Folder.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/EString.h"
#include "Core/Base/Exception.h"

namespace platform
{
class FolderException : public core::Exception
{
public:
    FolderException(core::StringView msg) : core::Exception(msg) {}
};


class Folder
{
public:
    Folder(core::StringView path) : path_(path) {}

    static bool IsFolderEmpty(core::StringView path);

    // clang-format off
    static core::Array<core::String> ListFiles(core::StringView path, bool recursive = false);
    static core::Array<core::String> ListFilesRegex(core::StringView path, core::StringView regex, bool recursive = false);
    static core::Array<core::String> ListFilesLambda(core::StringView path, const core::Function<bool(core::StringView)> &Func_Filter, bool recursive = false);

    static bool TryListFiles(core::StringView path, core::Array<core::String>& files, bool recursive = false);
    static bool TryListFilesRegex(core::StringView path, core::Array<core::String>& files, core::StringView regex, bool recursive = false);
    static bool TryListFilesLambda(core::StringView path, core::Array<core::String>& files, const core::Function<bool(core::StringView)> &Func_Filter, bool recursive = false);
    // clang-format on
    static bool ContainsFile(core::StringView path, core::StringView name, bool recursive = false);
    static bool CreateFolder(core::StringView path);

    [[nodiscard]] bool Create() const;
    [[nodiscard]] bool IsExist() const;

    [[nodiscard]] core::String GetAbsolutePath() const;
    [[nodiscard]] core::String GetRelativePath() const;

private:
    core::String path_;
};
}   // namespace platform
