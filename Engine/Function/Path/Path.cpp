/**
 * @file Path.cpp
 * @author Echo 
 * @Date 24-4-15
 * @brief 
 */

#include "Path.h"

#include "Exception.h"

Path::Path(StringView PathStr) {
    if (sProjectWorkPath == nullptr)
        throw ProjectPathNotValidException(String{PathStr.begin(), PathStr.end()});
    mPath = sProjectWorkPath->mPath / PathStr;
}

void Path::SetProjectWorkPath(const StringView PathStr) {
    sProjectWorkPath        = new Path{};
    sProjectWorkPath->mPath = PathStr;
    if (!sProjectWorkPath->IsExist()) {
        sProjectWorkPath->CreateDirectory();
        GetProjectMetaFilePath()->CreateFile();
    } else {
        if (!sProjectWorkPath->IsFolderEmpty()) {
            // TODO: 工程目录不为空则判断是否是工程目录
        }
        // TODO: 工程目录为空则创建工程元文件
    }

}

bool Path::IsExist() const noexcept {
    return std::filesystem::exists(mPath);
}

String Path::ToString() const noexcept {
    return mPath.generic_wstring();
}

void Path::CreateDirectory() const {
    std::error_code ec;
    create_directories(mPath, ec);
    if (ec) {
        throw PathInvalidException(*this, L"创建目录失败");
    }
}

bool Path::IsFolderEmpty() const noexcept {
    if (!IsFolder()) return false;
    // 判断mPath目录是不是空
    return std::filesystem::is_empty(mPath);
}

bool Path::IsFolder() const noexcept {
    return std::filesystem::is_directory(mPath);
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
