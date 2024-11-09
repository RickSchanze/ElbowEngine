/**
 * @file Folder.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "Base/CoreTypeDef.h"
#include "Base/EString.h"

namespace platform
{
class Folder
{
public:
    Folder(core::StringView path) : path_(path) {}

    static bool                      IsFolderEmpty(core::StringView path);
    static core::Array<core::String> ListFiles(core::StringView path, bool recursive = false);
    static core::Array<core::String> ListFiles(core::StringView path, core::StringView regex, bool recursive = false);
    static bool                      TryListFiles(core::StringView path, core::Array<core::String>& files, bool recursive = false);
    static bool TryListFiles(core::StringView path, core::Array<core::String>& files, core::StringView regex, bool recursive = false);
    static bool ContainsFile(core::StringView path, core::StringView name, bool recursive = false);

    void CreateFolder() const;

private:
    core::String path_;
};
}   // namespace platform
