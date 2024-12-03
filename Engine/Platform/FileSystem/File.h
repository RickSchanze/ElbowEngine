/**
 * @file File.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/EString.h"
#include "Core/Reflection/ITypeGetter.h"

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
    File(const core::String& path) : path_(std::move(path)) {}

    static bool Create_File(core::StringView path, FileCreateMode mode = FileCreateMode::Text, bool create_folder = true, bool overwrite = true);

    [[nodiscard]] core::String GetFolder() const;
    [[nodiscard]] core::String GetFileName() const;
    [[nodiscard]] core::String GetFileNameWithoutExtension() const;
    [[nodiscard]] core::String GetAbsolutePath() const;
    [[nodiscard]] core::String GetRelativePath() const;

    [[nodiscard]] bool IsExist() const;

    bool                                      TryReadAllText(core::String& out) const;
    [[nodiscard]] std::optional<core::String> ReadAllText() const;
    static core::Optional<core::String>       ReadAllText(const core::String& path);
    [[nodiscard]] bool                        WriteAllText(core::StringView text) const;
    static bool                               WriteAllText(const core::String& path, core::StringView text);

    [[nodiscard]] bool Create(FileCreateMode mode = FileCreateMode::Text, bool create_folder = true, bool overwrite = true) const;

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
