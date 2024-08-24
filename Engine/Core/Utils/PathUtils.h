/**
 * @file PathUtils.h
 * @author Echo 
 * @Date 24-8-24
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Path/Path.h"

enum class EPathFilterType
{
    FilterFile,
    FilterFolder,
    FilterAll,
};

inline auto FilterAlways = [](const Path& p) { return true; };
inline auto FilterNever  = [](const Path& p) { return false; };

class PathUtils
{
public:
    /**
     * 遍历parent文件夹 寻找符合条件的Path
     * @param parent
     * @param filter 一个接受代表Path的lambda函数
     * @param filter_type 参与过滤的类型 FilterFile/FilterFolder/FilterAll
     * @param recurse 是否递归查找子文件夹
     * @return
     */
    static TArray<Path> FilterPath(
        const Path& parent, const TFunction<bool(const Path& path_str)> &filter = FilterAlways, EPathFilterType filter_type = EPathFilterType::FilterAll,
        bool recurse = false
    );
};
