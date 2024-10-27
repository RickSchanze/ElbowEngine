/**
 * @file FileSystemError.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once
#include "CoreGlobal.h"

enum class FileSystemError
{
    NoError,
    NotExist,
    Busy,
    FileEOF,
    Count
};

enum class FileOpenMode
{
    Read,
    Write,
    ReadWrite,
    Append,
    Count
};

// template<>
// constexpr const char* GetEnumString<FileSystemError>(FileSystemError type)
// {
//     switch (type)
//     {
//     case FileSystemError::NotExist: return "NotExist";
//     case FileSystemError::NoError: return "NoError";
//     case FileSystemError::Busy: return "Busy";
//     case FileSystemError::FileEOF: return "FileEOF";
//     case FileSystemError::Count: return ENUM_INVALID;
//     }
//     return ENUM_OUT_OF_RANGE;
// }
//
// template<>
// constexpr const char* GetEnumString<FileOpenMode>(FileOpenMode type)
// {
//     switch (type)
//     {
//     case FileOpenMode::Read: return "Read";
//     case FileOpenMode::Write: return "Write";
//     case FileOpenMode::ReadWrite: return "ReadWrite";
//     case FileOpenMode::Count: return ENUM_INVALID;
//     }
//     return ENUM_OUT_OF_RANGE;
// }
