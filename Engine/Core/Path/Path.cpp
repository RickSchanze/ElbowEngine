/**
 * @file Path.cpp
 * @author Echo 
 * @Date 24-4-15
 * @brief 
 */

#include "Path.h"

#include "CoreEvents.h"
#include "Exception.h"
#include "Utils/StringUtils.h"

Path::Path(StringView PathStr)
{
    if (PathStr.back() == L'/' || PathStr.back() == L'\\')
    {
        // 去掉最后的'/'或者'\\'
        PathStr.remove_suffix(1);
    }
    path_ = PathStr;
}

#undef CreateFile
#undef CreateDirectory

void Path::SetProjectWorkPath(StringView PathStr)
{
    if (PathStr.back() == L'/' || PathStr.back() == L'\\')
    {
        // 去掉最后的'/'或者'\\'
        PathStr.remove_suffix(1);
    }
    s_project_work_path_        = new Path{};
    s_project_work_path_->path_ = PathStr;
    if (!s_project_work_path_->IsExist())
    {
        s_project_work_path_->CreateDirectory();
        GetProjectMetaFilePath()->CreateFile();
    }
    else
    {
        if (!s_project_work_path_->IsFolderEmpty())
        {
            // 查找存不存在工程元文件
            if (!GetProjectMetaFilePath()->IsExist())
            {
                throw PathInvalidException(*s_project_work_path_, L"目录不为空");
            }
        }
        else
        {
            GetProjectMetaFilePath()->CreateFile();
        }
    }
    OnProjectPathSet.Broadcast();
    OnProjectPathSet.Clear();
}

bool Path::IsExist() const
{
    return exists(GetStdFullPath());
}

void Path::CreateDirectory() const
{
    std::error_code ec;
    create_directories(GetStdFullPath(), ec);
    if (ec)
    {
        const auto ErrorMessage =
            std::format(L"创建目录失败,错误码:{},错误消息:{}", ec.value(), StringUtils::FromAnsiString(ec.message(), EStringEncoding::GBK));
        throw PathInvalidException(*this, ErrorMessage);
    }
}

bool Path::IsFolderEmpty() const
{
    // 判断此目录是否为空 则此目录必须存在
    if (!IsFolder(true)) return false;
    // 判断mPath目录是不是空
    return is_empty(GetStdFullPath());
}

bool Path::IsFolder(const bool bMustExist) const
{
    if (bMustExist)
    {
        if (!IsExist()) return false;
    }
    // 最后按理说不会为'/'或'\'因为构造函数已经去掉了
    std::filesystem::path TempPath = GetStdFullPath();
    return TempPath.has_filename() && !TempPath.has_extension();
}

TOptional<Path> Path::GetWorkPath() noexcept
{
    if (s_project_work_path_ == nullptr) return {};
    return *s_project_work_path_;
}

TOptional<Path> Path::GetProjectMetaFilePath() noexcept
{
    if (s_project_work_path_ == nullptr) return std::nullopt;
    const auto ProjectName = s_project_work_path_->path_.stem().generic_wstring();
    return Path(ProjectName + L".project");
}

void Path::CreateFile() const
{
    // 创建文件
    const std::ofstream file(path_);
    if (!file.is_open())
    {
        throw PathInvalidException(*this, L"创建文件失败");
    }
}

Path Path::GetParentPath() const
{
    return FromStdPath(GetStdFullPath().parent_path());
}

String Path::ToAbsoluteString() const
{
    return GetStdFullPath().generic_wstring();
}

String Path::ToRelativeString() const
{
    return path_.generic_wstring();
}

const char* Path::ToRelativeCStr()
{
    if (ansi_string_cache_.empty())
    {
        ansi_string_cache_ = ToRelativeAnsiString();
    }
    return ansi_string_cache_.c_str();
}

AnsiString Path::ToAbsoluteAnsiString() const
{
    return GetStdFullPath().generic_string();
}

AnsiString Path::ToRelativeAnsiString() const
{
    return path_.generic_string();
}

std::filesystem::path Path::GetStdFullPath() const{
    return s_project_work_path_->path_ / path_;
}
