/**
 * @file IResource.h
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#pragma once
#include "Core/Base/Interface.h"
#include "Core/Core.h"

#include GEN_HEADER("Resource.IResource.generated.h")

class CLASS(Interface) IResource : implements core::Interface
{
    GENERATED_CLASS(IResource)
public:
    ~IResource() override = default;

    [[nodiscard]] virtual core::String GetRelativePath() const = 0;
    [[nodiscard]] virtual core::String GetAbsolutePath() const = 0;
    [[nodiscard]] virtual bool         IsValid() const         = 0;
    virtual void                       Load()                  = 0;
};
