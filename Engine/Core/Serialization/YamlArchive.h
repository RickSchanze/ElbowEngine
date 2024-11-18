/**
 * @file YamlArchive.h
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#pragma once
#include "Archive.h"
#include "Core/Base/Base.h"

namespace YAML
{
class Node;
class Emitter;
}   // namespace YAML
namespace core
{
class YamlArchive : public Archive
{
public:
    bool Serialize(const Any& obj, String& out) override;
    bool Deserialize(core::StringView source, void* out, const Type* type) override;
};
}   // namespace core
