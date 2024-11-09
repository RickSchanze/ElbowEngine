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


namespace platform
{

class File
{
public:
    File(const core::String& path) : path_(path) {}

    [[nodiscard]] core::String GetFolder() const;
    [[nodiscard]] core::String GetFileName() const;
    [[nodiscard]] core::String GetFileNameWithoutExtension() const;
    [[nodiscard]] core::String GetAbsolutePath() const;
    [[nodiscard]] core::String GetRelativePath() const;

    [[nodiscard]] bool IsExist() const;

    bool operator==(const File& f) const { return path_ == f.path_; }

private:
    // 相对于项目工程路径的路径
    core::String path_;
};
}
