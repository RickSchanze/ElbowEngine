/**
 * @file Exception.h
 * @author Echo 
 * @Date 24-4-15
 * @brief 
 */

#pragma once

#include <utility>

#include "Exception/Exception.h"
#include "Path.h"

class PathInvalidException final : public Exception {
public:
    explicit PathInvalidException(const Path& Path, String Message = L"") :
        Exception{std::move(Message)}, mPath{Path} {}

    [[nodiscard]] const Path& GetPath() const noexcept { return mPath; }

    [[nodiscard]] String What() const override;

private:
    Path mPath;
};

class ProjectPathNotValidException final : public Exception {
public:
    explicit ProjectPathNotValidException(const String& PathStr, String Message = L"") :
        Exception{std::move(Message)}, mPath{PathStr} {}

    [[nodiscard]] const Path& GetPath() const noexcept { return mPath; }

    [[nodiscard]] String What() const override;

private:
    Path mPath;
};

class FileCreateFailedException final : public Exception {
public:
    explicit FileCreateFailedException(const String& PathStr, String Message = L"") :
        Exception{std::move(Message)}, mPath{PathStr} {}

    [[nodiscard]] const Path& GetPath() const noexcept { return mPath; }

    [[nodiscard]] String What() const override;

private:
    Path mPath;
};
