/**
 * @file Guid.h
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#pragma once
#include "CoreDef.h"

#include <objbase.h>

class Guid
{
public:
    Guid();

    AnsiString ToAnsiString() const;
    String     ToString() const;

private:
    GUID                  guid_{};
    static constexpr int8_t length = 64;
};
