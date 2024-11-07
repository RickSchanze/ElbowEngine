/**
 * @file YamlArchive.cpp
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#include "YamlArchive.h"

#include "CoreGlobal.h"
#include "ISerializer.h"
#include "Log/CoreLogCategory.h"
#include "yaml-cpp/yaml.h"

#define IMPL_BASE_TYPE \
    *emitter_ << i;    \
    return *this

namespace core
{
YamlArchive::~YamlArchive() {}

Archive& YamlArchive::operator<<(ISerializer& serializer)
{
    // Assert(Archive.Serialization, IsSerializing(), "请在Serializing模式使用此函数");
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

Archive& YamlArchive::operator<<(const String& str)
{
    *emitter_ << str.StdString();
    return *this;
}

Archive& YamlArchive::operator<<(const StringView& str)
{
    *emitter_ << String(str);
    return *this;
}

Archive& YamlArchive::operator<<(InputType type)
{
    // Assert(Archive.Serialization, IsSerializing(), "请在Serializing模式使用此函数");
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
    switch (state_)
    {
    case State::Serialized: emitter_.Release(); break;
    case State::Deserialized: node_.Release(); break;
    case State::Serializing:
    case State::Deserializing: LOGGER.Error(LogCat::Archive_Serialization, "处于不用的状态: {}", GetEnumString(state_)); break;
    case State::Idle: break;
    }
    emitter_ = MakeUnique<YAML::Emitter>();
    Archive::BeginSerialize();
}

void YamlArchive::EndSerialize()
{
    Archive::EndSerialize();
}

String YamlArchive::ToString()
{
    if (HasError())
    {
        return "Some error occured during serialization";
    }
    if (IsSerialized())
    {
        return emitter_->c_str();
    }
    return "只有被序列化的数据才能转换为字符串";
}
}
