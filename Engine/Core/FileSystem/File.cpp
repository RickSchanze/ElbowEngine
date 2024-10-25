/**
 * @file File.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#include "File.h"

#include <filesystem>

using namespace std::filesystem;

FileSystemError File::Check() const
{
    path p = *file_path_;
    if (!exists(p))
    {
        return FileSystemError::NotExist;
    }
    return FileSystemError::NoError;
}

bool File::IsExist() const
{
    return Check() == FileSystemError::NoError;
}

File::~File()
{
    Close();
}

FileSystemError File::Open(FileOpenMode mode)
{
    if (!IsExist())
    {
        return FileSystemError::NotExist;
    }
    switch (mode)
    {
    case FileOpenMode::Read: stream_.open(*file_path_, std::ios::in); break;
    case FileOpenMode::Write: stream_.open(*file_path_, std::ios::out); break;
    case FileOpenMode::ReadWrite: stream_.open(*file_path_, std::ios::in | std::ios::out); break;
    case FileOpenMode::Append: stream_.open(*file_path_, std::ios::app); break;
    case FileOpenMode::Count: return FileSystemError::Count;
    }
    if (!stream_.is_open())
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            return FileSystemError::Busy;
        }
    }
    return FileSystemError::NoError;
}

void File::Close()
{
    if (stream_.is_open())
    {
        stream_.close();
    }
}

std::expected<String, FileSystemError> File::ReadAll()
{
    if (!IsOpen())
    {
        FileSystemError e = Open(FileOpenMode::Read);
        if (e != FileSystemError::NoError)
        {
            return std::unexpected(e);
        }
    }
    std::string str((std::istreambuf_iterator(stream_)), std::istreambuf_iterator<char>());
    return str;
}

std::expected<String, FileSystemError> File::ReadLine()
{
    if (stream_.eof())
    {
        return std::unexpected(FileSystemError::FileEOF);
    }
    std::string str;
    std::getline(stream_, str);
    return str;
}

bool File::IsOpen() const
{
    return stream_.is_open();
}
