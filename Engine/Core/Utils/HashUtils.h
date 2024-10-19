/**
 * @file HashUtils.h
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#pragma once

#include "Path/Path.h"


class HashUtils {
public:
    static Optional<AnsiString> ComputeSHA256(const Path& path);
};
