/**
 * @file File.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "Base/EString.h"
#include "Reflection/ITypeGetter.h"
#include "Reflection/MetaInfoMacro.h"
#include <utility>


namespace platform
{

enum class FileSystemError
{
    Success,
    FileNotFound,
    FailedToOpenFile
};

enum class FileCreateMode
{
    Text,
    Binary
};

class File
{
public:
    File() = default;
    File(core::String path) : path_(std::move(path)) {}

    static bool Create_File(core::StringView path, FileCreateMode mode = FileCreateMode::Text, bool combine_proj_path = true);

    [[nodiscard]] core::String GetFolder() const;
    [[nodiscard]] core::String GetFileName() const;
    [[nodiscard]] core::String GetFileNameWithoutExtension() const;
    [[nodiscard]] core::String GetAbsolutePath() const;
    [[nodiscard]] core::String GetRelativePath() const;

    [[nodiscard]] bool IsExist() const;

    FileSystemError            TryReadAllText(core::String& out, bool combine_proj_path = true) const;
    [[nodiscard]] core::String ReadAllText(bool combine_proj_path = true) const;

    [[nodiscard]] bool Create(FileCreateMode mode = FileCreateMode::Text, bool combine_proj_path = true) const;

    bool operator==(const File& f) const { return path_ == f.path_; }

private:
    // 相对于项目工程路径的路径
    core::String path_;
};
}   // namespace platform

template <>
struct std::hash<platform::File>
{
    size_t operator()(const platform::File& f) const noexcept { return std::hash<core::String>()(f.GetAbsolutePath()); }
};
