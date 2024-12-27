/**
 * @file Path.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/EString.h"
#include "File.h"

namespace platform
{
class Path
{
public:
    static core::String     Combine(core::StringView left, core::StringView right);
    static core::StringView GetProjectPath();
    static bool             SetProjectPath(core::StringView path);
    static bool             IsExist(core::StringView path);
    static bool             IsFolder(core::StringView path);

    /**
     * 获取path的父文件夹路径
     * @param path path是一个相对路径, 如果是顶层, 则返回项目路径, 如果不是顶层则返回上一级路径
     * @return
     */
    static core::String     GetParent(core::StringView path);

    static core::StringView GetFileName(core::StringView file_path);

    static core::StringView GetFileNameWithoutExt(core::StringView file_path);

private:
    static inline core::String s_proj_path_;
};
}   // namespace platform
