/**
 * @file YamlArchive.cpp
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#include "YamlArchive.h"

#include "CoreGlobal.h"
#include "ISerializer.h"
#include "yaml-cpp/yaml.h"

#define IMPL_BASE_TYPE                                                               \
    Assert(Archive.Serialization, IsSerializing(), "请在Serializing模式使用此函数"); \
    *emitter_ << i;                                                                  \
    return *this

YamlArchive::~YamlArchive() {}

Archive& YamlArchive::operator<<(ISerializer& serializer)
{
    Assert(Archive.Serialization, IsSerializing(), "请在Serializing模式使用此函数");
    serializer.Serialize(*this);
    return *this;
}

Archive& YamlArchive::operator<<(int8_t i)
{
    IMPL_BASE_TYPE;
}

Archive& YamlArchive::operator<<(int16_t i)
{
    IMPL_BASE_TYPE;
}

Archive& YamlArchive::operator<<(int32_t i)
{
    IMPL_BASE_TYPE;
}
Archive& YamlArchive::operator<<(int64_t i)
{
    IMPL_BASE_TYPE;
}
Archive& YamlArchive::operator<<(uint8_t i)
{
    IMPL_BASE_TYPE;
}
Archive& YamlArchive::operator<<(uint16_t i)
{
    IMPL_BASE_TYPE;
}

Archive& YamlArchive::operator<<(uint32_t i)
{
    IMPL_BASE_TYPE;
}

Archive& YamlArchive::operator<<(uint64_t i)
{
    IMPL_BASE_TYPE;
}
Archive& YamlArchive::operator<<(bool i)
{
    IMPL_BASE_TYPE;
}
Archive& YamlArchive::operator<<(double i)
{
    IMPL_BASE_TYPE;
}
Archive& YamlArchive::operator<<(float i)
{
    IMPL_BASE_TYPE;
}
Archive& YamlArchive::operator<<(const char* i)
{
    IMPL_BASE_TYPE;
}

Archive& YamlArchive::operator<<(InputType type)
{
    Assert(Archive.Serialization, IsSerializing(), "请在Serializing模式使用此函数");
    switch (type)
    {
    case InputType::ArrayStart: *emitter_ << YAML::BeginSeq; break;
    case InputType::ArrayEnd: *emitter_ << YAML::EndSeq; break;
    case InputType::MapStart: *emitter_ << YAML::BeginMap; break;
    case InputType::MapEnd: *emitter_ << YAML::EndMap; break;
    case InputType::Key: *emitter_ << YAML::Key; break;
    case InputType::Value: *emitter_ << YAML::Value; break;
    }
    return *this;
}

void YamlArchive::BeginSerialize()
{
    Archive::BeginSerialize();
    emitter_ = MakeUnique<YAML::Emitter>();
}

void YamlArchive::EndSerialize()
{
    Archive::EndSerialize();
    emitter_.Reset();
}

AnsiString YamlArchive::ToString()
{
    if (IsSerialized())
    {
        return emitter_->c_str();
    }
    return "只有被序列化的数据才能转换为字符串";
}
