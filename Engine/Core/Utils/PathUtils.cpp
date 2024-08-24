/**
 * @file PathUtils.cpp
 * @author Echo 
 * @Date 24-8-24
 * @brief 
 */

#include "PathUtils.h"
#include "CoreGlobal.h"

TArray<Path> PathUtils::FilterPath(const Path& parent, const TFunction<bool(const Path& p)>& filter, EPathFilterType filter_type, bool recurse)
{
    if (!parent.IsExist())
    {
        LOG_ERROR_CATEGORY(Path, L"{}: 传入路径不存在", parent.ToRelativeString());
        return {};
    }
    if (!parent.IsFolder())
    {
        LOG_ERROR_CATEGORY(Path, L"{}: 传入路径应该是一个目录而不是文件", parent.ToRelativeString());
        return {};
    }
    TArray<Path> rtn;
    if (recurse)
    {
        auto iterator = std::filesystem::recursive_directory_iterator(parent.ToAbsoluteAnsiString());
        for (const auto& entry: iterator)
        {
            Path p = Path::FromStdPath(entry.path());
            switch (filter_type)
            {
            case EPathFilterType::FilterFile:
                if (!p.IsFolder())
                {
                    if (filter(p))
                    {
                        rtn.push_back(p);
                    }
                }
                break;
            case EPathFilterType::FilterFolder:
                if (p.IsFolder())
                {
                    if (filter(p))
                    {
                        rtn.push_back(p);
                    }
                }
                break;
            case EPathFilterType::FilterAll:
                if (filter(p))
                {
                    rtn.push_back(p);
                }
                break;
            }
        }
    }
    else
    {
        auto iterator = std::filesystem::directory_iterator(parent.ToAbsoluteAnsiString());
        for (const auto& entry: iterator)
        {
            Path p = Path::FromStdPath(entry.path());
            switch (filter_type)
            {
            case EPathFilterType::FilterFile:
                if (!p.IsFolder())
                {
                    if (filter(p))
                    {
                        rtn.push_back(p);
                    }
                }
                break;
            case EPathFilterType::FilterFolder:
                if (p.IsFolder())
                {
                    if (filter(p))
                    {
                        rtn.push_back(p);
                    }
                }
                break;
            case EPathFilterType::FilterAll:
                if (filter(p))
                {
                    rtn.push_back(p);
                }
                break;
            }
        }
    }
    return rtn;
}
