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
    void Serialize(const Any& obj, String& out) override;
    void Deserialize(core::StringView source, Ref<void*> out) override;
};
}   // namespace core
