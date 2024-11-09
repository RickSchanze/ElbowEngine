/**
 * @file Archive.h
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#pragma once
#include "Base/Base.h"
#include "CoreDef.h"
#include "CoreGlobal.h"
#include "CoreTypeTraits.h"
#include "Log/CoreLogCategory.h"

namespace core
{
struct Any;
}
namespace core
{
class ISerializer;

class Archive
{
public:
    enum class State
    {
        Serializing,
        Deserializing,
        Serialized,
        Deserialized,
        Idle,
        Error,
    };

    enum class InputType
    {
        ArrayStart,
        ArrayEnd,
        MapStart,
        MapEnd,
        Key,
        Value,
    };

    virtual ~Archive() = default;

    virtual Archive& operator<<(InputType type)          = 0;
    virtual Archive& operator<<(ISerializer& serializer) = 0;

#if REGION(基础类型)
    virtual Archive& operator<<(int8_t i)              = 0;
    virtual Archive& operator<<(int16_t i)             = 0;
    virtual Archive& operator<<(int32_t i)             = 0;
    virtual Archive& operator<<(int64_t i)             = 0;
    virtual Archive& operator<<(uint8_t i)             = 0;
    virtual Archive& operator<<(uint16_t i)            = 0;
    virtual Archive& operator<<(uint32_t i)            = 0;
    virtual Archive& operator<<(uint64_t i)            = 0;
    virtual Archive& operator<<(bool b)                = 0;
    virtual Archive& operator<<(double d)              = 0;
    virtual Archive& operator<<(float f)               = 0;
    virtual Archive& operator<<(const String& str)     = 0;
    virtual Archive& operator<<(const StringView& str) = 0;
#endif

    Archive& operator<<(const Any& value);

    template <typename Enum>
        requires std::is_enum_v<Enum>
    Archive& operator<<(Enum e)
    {
        return GetEnumString(e);
    }

#if REGION(容器类型)
    template <template <typename, typename...> class Container, typename Element>
    Archive& operator<<(const Container<Element>& container)
        requires ArrayLikeIterable<Container<Element>>
    {
        DebugAssert(logcat::Archive_Serialization, IsSerializing(), "请在Serializing模式使用此函数");
        if (container.size() != 0)
        {
            *this << InputType::ArrayStart;
            for (const auto& item: container)
            {
                *this << item;
            }
            *this << InputType::ArrayEnd;
        }
        return *this;
    }

    template <template <typename...> typename Map, typename Key, typename Value>
    Archive& operator<<(const Map<Key, Value>& container)
        requires MapLikeIterable<Map<Key, Value>>
    {
        Assert(logcat::Archive_Serialization, IsSerializing(), "请在Serializing模式使用此函数");
        if (container.size() != 0)
        {
            *this << InputType::MapStart;
            for (const auto& item: container)
            {
                *this << InputType::Key;
                *this << item.first;
                *this << InputType::Value;
                *this << item.second;
            }
            *this << InputType::MapEnd;
        }
        return *this;
    }
#endif

    [[nodiscard]] State GetState() const { return state_; }
    [[nodiscard]] bool  IsSerializing() const { return state_ == State::Serializing; }
    [[nodiscard]] bool  IsDeserializing() const { return state_ == State::Deserializing; }
    [[nodiscard]] bool  IsSerialized() const { return state_ == State::Serialized; }
    [[nodiscard]] bool  IsDeserialized() const { return state_ == State::Deserialized; }

    [[nodiscard]] virtual String ToString() = 0;

    virtual void BeginSerialize() { state_ = State::Serializing; }
    virtual void EndSerialize() { state_ = State::Serialized; }
    virtual void BeginDeserialize() { state_ = State::Deserializing; }
    virtual void EndDeserialize() { state_ = State::Deserialized; }

    void SetError() { state_ = State::Error; }

    [[nodiscard]] bool HasError() const { return state_ == State::Error; }

protected:
    State state_ = State::Idle;
};
}   // namespace core

template <>
inline core::StringView GetEnumString<core::Archive::State>(core::Archive::State value)
{
    switch (value)
    {
    case core::Archive::State::Idle: return "Idle";
    case core::Archive::State::Serializing: return "Serializing";
    case core::Archive::State::Serialized: return "Serialized";
    case core::Archive::State::Deserializing: return "Deserializing";
    case core::Archive::State::Deserialized: return "Deserialized";
    default: return ENUM_INVALID;
    }
}
