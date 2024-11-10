/**
 * @file IResource.h
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#pragma once
#include "Base/EString.h"

class IResource
{
public:
    virtual ~IResource() = default;

    [[nodiscard]] virtual core::StringView GetRelativePath() const = 0;
    [[nodiscard]] virtual core::StringView GetAbsolutePath() const = 0;
    [[nodiscard]] virtual bool             IsValid() const         = 0;
    virtual void                           Load()                  = 0;
};
