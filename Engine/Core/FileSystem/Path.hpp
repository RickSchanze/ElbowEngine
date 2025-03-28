//
// Created by Echo on 2025/3/21.
//


#pragma once
#include <filesystem>

#include "Core/Event/Event.hpp"

struct OnProjectPathSetEvent : MulticastEvent<void, StringView> {};

bool IsProjectPathValid(StringView path);

inline OnProjectPathSetEvent Evt_OnProjectPathSet;

class Path {
public:
    static String Combine(StringView left, StringView right);
    static StringView GetProjectPath() { return s_proj_path_; }

    static bool SetProjectPath(StringView path);

    static bool IsExist(StringView path) { return exists(std::filesystem::path(String(path).ToWideString())); }

    static bool IsFolder(StringView path);

    /**
     * 获取path的父文件夹路径
     * @param path path是一个相对路径, 如果是顶层, 则返回项目路径, 如果不是顶层则返回上一级路径
     * @return
     */
    static String GetParent(StringView path);

    static StringView GetFileName(StringView file_path);

    static StringView GetFileNameWithoutExt(StringView file_path);

    static String GetAbsolutePath(StringView path);

private:
    static inline String s_proj_path_;
};
