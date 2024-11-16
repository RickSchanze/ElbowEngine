/**
 * @file Archive.h
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#pragma once

#include "Core/Base/Ref.h"
#include "Core/CoreGlobal.h"

namespace core
{
struct Any;
class ITypeGetter;
}
namespace core
{
class ISerializer;

class Archive
{
public:
    virtual ~Archive() = default;

    virtual void Serialize(const Any& obj, String& out)             = 0;
    virtual void Deserialize(core::StringView source, Ref<void*> out) = 0;
};
}   // namespace core
