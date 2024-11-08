/**
 * @file ISerializer.h
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#pragma once
#include "Base/Interface.h"
#include "Reflection/MetaInfoMacro.h"
#include "Reflection/MetaInfoManager.h"

#include "Core.ISerializer.generated.h"

namespace core
{
class Archive;

class CLASS(Interface) ISerializer : public Interface
{
    GENERATED_BODY(ISerializer)
public:
    ISerializer() = default;
    ~ISerializer() override = default;

    virtual void Serialize(Archive& ar) = 0;
};
}   // namespace core
