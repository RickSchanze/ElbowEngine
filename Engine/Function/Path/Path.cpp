/**
 * @file Path.cpp
 * @author Echo 
 * @Date 24-4-15
 * @brief 
 */

#include "Path.h"

#include "Exception.h"
#include "Utils/StringUtils.h"

Path::Path(StringView PathStr) {
    if (PathStr.back() == L'/' || PathStr.back() == L'\\') {
        // 去掉最后的'/'或者'\\'
        PathStr.remove_suffix(1);
    }
    if (sProjectWorkPath == nullptr)
        throw ProjectPathNotValidException(String{PathStr.begin(), PathStr.end()});
    mPath = sProjectWorkPath->mPath / PathStr;
}

void Path::SetProjectWorkPath(StringView PathStr) {
    if (PathStr.back() == L'/' || PathStr.back() == L'\\') {
        // 去掉最后的'/'或者'\\'
        PathStr.remove_suffix(1);
    }
    sProjectWorkPath        = new Path{};
    sProjectWorkPath->mPath = PathStr;
    if (!sProjectWorkPath->IsExist()) {
        sProjectWorkPath->CreateDirectory();
        GetProjectMetaFilePath()->CreateFile();
    } else {
        if (!sProjectWorkPath->IsFolderEmpty()) {
            // 查找存不存在工程元文件
            if (!GetProjectMetaFilePath()->IsExist()) {
                throw PathInvalidException(*sProjectWorkPath, L"目录不为空");
            }
        } else {
            GetProjectMetaFilePath()->CreateFile();
        }
    }
}

bool Path::IsExist() const {
    return std::filesystem::exists(mPath);
}

String Path::ToString() const {
    return mPath.generic_wstring();
}

void Path::CreateDirectory() const {
    std::error_code ec;
    create_directories(mPath, ec);
    if (ec) {
        const auto ErrorMessage = std::format(
            L"创建目录失败,错误码:{},错误消息:{}",
            ec.value(),
            StringUtils::FromAnsiString(ec.message(), EStringEncoding::GBK)
        );
        throw PathInvalidException(*this, ErrorMessage);
    }
}

bool Path::IsFolderEmpty() const {
    // 判断此目录是否为空 则此目录必须存在
    if (!IsFolder(true)) return false;
    // 判断mPath目录是不是空
    return std::filesystem::is_empty(mPath);
}

bool Path::IsFolder(const bool bMustExist) const {
    if (bMustExist) {
        if (!IsExist()) return false;
    }
    // 最后按理说不会为'/'或'\'因为构造函数已经去掉了
    return mPath.has_filename() && !mPath.has_extension();
}

Optional<Path> Path::GetWorkPath() noexcept {
    if (sProjectWorkPath == nullptr) return {};
    return *sProjectWorkPath;
}

Optional<Path> Path::GetProjectMetaFilePath() noexcept {
    if (sProjectWorkPath == nullptr) return std::nullopt;
    const auto ProjectName = sProjectWorkPath->mPath.stem().generic_wstring();
    return Path(ProjectName + L".project");
}

void Path::CreateFile() const {
    // 创建文件
    const std::ofstream file(mPath);
    if (!file.is_open()) {
        throw PathInvalidException(*this, L"创建文件失败");
    }
}