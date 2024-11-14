/**
 * @file YamlArchive.h
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#pragma once
#include "Archive.h"
#include "Base/Base.h"

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
    ~YamlArchive() override;
#if REGION(基础类型)
    Archive& operator<<(ISerializer& serializer) override;
    Archive& operator<<(int8_t i) override;
    Archive& operator<<(int16_t i) override;
    Archive& operator<<(int32_t i) override;
    Archive& operator<<(int64_t i) override;
    Archive& operator<<(uint8_t i) override;
    Archive& operator<<(uint16_t i) override;
    Archive& operator<<(uint32_t i) override;
    Archive& operator<<(uint64_t i) override;
    Archive& operator<<(bool b) override;
    Archive& operator<<(double d) override;
    Archive& operator<<(float f) override;
    Archive& operator<<(const String& str) override;
    Archive& operator<<(const StringView& str) override;
#endif

    Archive& operator<<(InputType type) override;

    void BeginSerialize() override;
    void EndSerialize() override;

    Expected<String, ArchiveError> ToString() override;

private:
    union {
        core::UniquePtr<YAML::Node>    node_ = nullptr;
        core::UniquePtr<YAML::Emitter> emitter_;
    };
};
}


