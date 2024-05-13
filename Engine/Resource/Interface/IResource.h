/**
 * @file IResource.h
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#pragma once
#include "Path/Path.h"

class IResource {
public:
    virtual ~IResource() = default;

    virtual Path GetPath() const = 0;
    virtual bool IsValid() const = 0;
    virtual void Load() = 0;
};
