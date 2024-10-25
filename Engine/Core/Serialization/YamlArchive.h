/**
 * @file YamlArchive.h
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#pragma once
#include "Archive.h"

namespace YAML
{
class Node;
class Emitter;
}   // namespace YAML

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
    Archive& operator<<(const char* str) override;
#endif

    Archive& operator<<(InputType type) override;

    void BeginSerialize() override;
    void EndSerialize() override;

    AnsiString ToString() override;

private:
    union {
        UniquePtr<YAML::Node>    node_ = nullptr;
        UniquePtr<YAML::Emitter> emitter_;
    };
    bool serializing_ = true;
};
