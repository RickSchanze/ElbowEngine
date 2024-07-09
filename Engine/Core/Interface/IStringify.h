/**
 * @file IStringify.h
 * @author Echo 
 * @Date 24-7-9
 * @brief 
 */

#pragma once
#include "CoreDef.h"

class IStringify
{
public:
    virtual ~IStringify() = default;
    virtual String ToString() const = 0;
};

