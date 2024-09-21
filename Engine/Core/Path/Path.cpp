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
    s_project_work_path_        = New<Path>();
    s_project_work_path_->path_ = PathStr;
    if (!s_project_work_path_->IsExist())
    {
        s_project_work_path_->CreateDir();
        GetProjectMetaFilePath()->CreateFileA();
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
            GetProjectMetaFilePath()->CreateFileA();
        }
    }
    OnProjectPathSet.Broadcast();
    OnProjectPathSet.Clear();
}

bool Path::IsExist() const
{
    return exists(GetStdFullPath());
}

void Path::CreateDir() const
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

void Path::CreateFileA() const
{
    // 创建文件
    const std::ofstream file(ToAbsoluteAnsiString());
    if (!file.is_open())
    {
        LOG_ERROR_CATEGORY(Path, L"创建文件 {} 失败", ToRelativeString());
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

bool Path::EndsWith(const String& subfix) const
{
    return path_.extension().generic_wstring() == subfix;
}

String Path::GetFileName() const
{
    return path_.filename().generic_wstring();
}

AnsiString Path::ToAbsoluteAnsiString() const
{
    return GetStdFullPath().generic_string();
}

AnsiString Path::ToRelativeAnsiString() const
{
    return path_.generic_string();
}

AnsiString Path::ReadAllText() const
{
    if (IsFolder())
    {
        LOG_ERROR_CATEGORY(Path, L"在文件夹路径上调用了ReadAllText");
        return "";
    }
    std::ifstream file(ToAbsoluteAnsiString());
    if (!file.is_open())
    {
        LOG_ERROR_CATEGORY(Path, L"打开文件 {} 失败", ToRelativeString());
        return "";
    }
    AnsiString content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

void Path::ReadAllBinary(TArray<char>& output) const
{
    if (IsFolder())
    {
        LOG_ERROR_CATEGORY(Path, L"在文件夹路径上调用了ReadAllText");
        output.clear();
        return;
    }
    std::ifstream     shader_file_stream{ToAbsoluteAnsiString(), std::ios::ate | std::ios::binary};
    auto              shader_file_size = shader_file_stream.tellg();
    std::vector<char> shader_code(shader_file_size);
    shader_file_stream.seekg(0);
    shader_file_stream.read(shader_code.data(), shader_file_size);
    shader_file_stream.close();
    output = shader_code;
}

void Path::ReadAllBinary(TArray<uint32_t>& output) const
{
    if (IsFolder())
    {
        LOG_ERROR_CATEGORY(Path, L"在文件夹路径上调用了ReadAllText");
        output.clear();
        return;
    }
    std::ifstream     shader_file_stream{ToAbsoluteAnsiString(), std::ios::ate | std::ios::binary};
    auto              shader_file_size = shader_file_stream.tellg();
    if (shader_file_size % 4 != 0)
    {
        LOG_ERROR_CATEGORY(Path, L"此二进制不能被作为uint32_t读取 {}", ToAbsoluteString());
        output.clear();
        return;
    }
    shader_file_stream.seekg(0, std::ios::beg);
    size_t num_elements = shader_file_size / sizeof(uint32_t);
    output.resize(num_elements);
    shader_file_stream.read(reinterpret_cast<char*>(output.data()), shader_file_size);
    shader_file_stream.close();
}

void Path::WriteAllBinary(const TArray<char>& binary) const
{
    std::ofstream file(ToAbsoluteAnsiString(), std::ios::out | std::ios::binary);
    if (!file)
    {
        LOG_ERROR_CATEGORY(Path, "写二进制文件{}失败", ToRelativeString());
        return;
    }

    file.write(binary.data(), binary.size());
    if (!file)
    {
        LOG_ERROR_CATEGORY(Path, "写二进制文件{}失败", ToRelativeString());
        return;
    }

    file.close();
}

void Path::WriteAllBinary(const TArray<uint32_t>& binary) const
{
    std::ofstream file(ToAbsoluteAnsiString(), std::ios::out | std::ios::binary);
    if (!file)
    {
        LOG_ERROR_CATEGORY(Path, "写二进制文件{}失败", ToRelativeString());
        return;
    }

    file.write(reinterpret_cast<const char*>(binary.data()), 4 * binary.size());
    if (!file)
    {
        LOG_ERROR_CATEGORY(Path, "写二进制文件{}失败", ToRelativeString());
        return;
    }

    file.close();
}

void Path::WriteAllText(const AnsiString& text) const
{
    std::ofstream file(ToAbsoluteAnsiString(), std::ios::out);
    if (!file)
    {
        LOG_ERROR_CATEGORY(Path, "写二进制文件{}失败", ToRelativeString());
        return;
    }

    file.write(text.data(), text.size());
    if (!file)
    {
        LOG_ERROR_CATEGORY(Path, "写二进制文件{}失败", ToRelativeString());
        return;
    }

    file.close();
}

std::filesystem::path Path::GetStdFullPath() const{
    return s_project_work_path_->path_ / path_;
}
