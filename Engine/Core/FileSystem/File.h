/**
 * @file File.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once

#include <utility>

#include "FileSystemEnum.h"

#include <expected>

class File
{
public:
    explicit File(String file_path) : file_path_(std::move(file_path)) {}

    ~File();

    [[nodiscard]] FileSystemError Check() const;
    [[nodiscard]] bool            IsExist() const;
    [[nodiscard]] bool            IsOpen() const;
    [[nodiscard]] FileOpenMode    GetOpenMode() const { return open_mode_; }

    FileSystemError Open(FileOpenMode mode);
    void            Close();

    std::expected<String, FileSystemError> ReadAll();
    std::expected<String, FileSystemError> ReadLine();

private:
    String       file_path_;
    std::fstream stream_;
    FileOpenMode open_mode_ = FileOpenMode::Count;
};
