//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/CoreMacros.hpp"
#include "Core/String/String.hpp"
#include "Core/TypeTraits.hpp"

class OutputArchive
{
public:
    virtual ~OutputArchive() = default;

    virtual void WriteArraySize(UInt64 InSize) = 0;
    virtual void WriteMapSize(UInt64 InSize) = 0;

    virtual void WriteNumber(StringView InName, const Int8& Data) = 0;
    virtual void WriteNumber(StringView InName, const Int16& Data) = 0;
    virtual void WriteNumber(StringView InName, const Int32& Data) = 0;
    virtual void WriteNumber(StringView InName, const Int64& Data) = 0;
    virtual void WriteNumber(StringView InName, const UInt8& Data) = 0;
    virtual void WriteNumber(StringView InName, const UInt16& Data) = 0;
    virtual void WriteNumber(StringView InName, const UInt32& Data) = 0;
    virtual void WriteNumber(StringView InName, const UInt64& Data) = 0;
    virtual void WriteNumber(StringView InName, const Float& Data) = 0;
    virtual void WriteNumber(StringView InName, const Double& Data) = 0;

    virtual void WriteNumber(const Int8& Data) = 0;
    virtual void WriteNumber(const Int16& Data) = 0;
    virtual void WriteNumber(const Int32& Data) = 0;
    virtual void WriteNumber(const Int64& Data) = 0;
    virtual void WriteNumber(const UInt8& Data) = 0;
    virtual void WriteNumber(const UInt16& Data) = 0;
    virtual void WriteNumber(const UInt32& Data) = 0;
    virtual void WriteNumber(const UInt64& Data) = 0;
    virtual void WriteNumber(const Float& Data) = 0;
    virtual void WriteNumber(const Double& Data) = 0;

    virtual void WriteBool(StringView InName, const bool& Data) = 0;
    virtual void WriteBool(const bool& Data) = 0;

    // 不允许StringView序列化
    virtual void WriteString(StringView InName, const String& Data) = 0;
    virtual void WriteString(const String& Data) = 0;

    virtual void SetNextScopeName(StringView InName) = 0;
    virtual void BeginScope() = 0;
    virtual void EndScope() = 0;

    template <typename T>
        requires(!Traits::IsEnum<T>)
    ELBOW_FORCE_INLINE void WriteType(const StringView InName, const T& Value)
    {
        static_assert(!Traits::SameAs<T, StringView>, "StringView does not support serialization!");
        if constexpr (Traits::IsAnyOf<Pure<T>, Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64, Float, Double>)
        {
            WriteNumber(InName, Value);
        }
        else if constexpr (Traits::IsAnyOf<Pure<T>, bool>)
        {
            WriteBool(InName, Value);
        }
        else if constexpr (Traits::IsAnyOf<Pure<T>, String>)
        {
            WriteString(InName, Value);
        }
        else
        {
            SetNextScopeName(InName);
            BeginScope();
            Value.Serialization_Save(*this);
            EndScope();
        }
    }

    template <typename T>
        requires(!Traits::IsEnum<T>)
    ELBOW_FORCE_INLINE void WriteType(const T& Value)
    {
        static_assert(!Traits::SameAs<T, StringView>, "StringView does not support serialization!");
        if constexpr (Traits::IsAnyOf<Pure<T>, Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64, Float, Double>)
        {
            WriteNumber(Value);
        }
        else if constexpr (Traits::IsAnyOf<Pure<T>, bool>)
        {
            WriteBool(Value);
        }
        else if constexpr (Traits::IsAnyOf<Pure<T>, String>)
        {
            WriteString(Value);
        }
        else
        {
            BeginScope();
            Value.Serialization_Save(*this);
            EndScope();
        }
    }

    template <typename T>
        requires(Traits::IsEnum<T>)
    ELBOW_FORCE_INLINE void WriteType(const StringView InName, const T& Value)
    {
        const Int32 Tmp = static_cast<Int32>(std::to_underlying(Value));
        WriteNumber(InName, Tmp);
    }

    template <typename T>
        requires(Traits::IsEnum<T>)
    ELBOW_FORCE_INLINE void WriteType(const T& Value)
    {
        const Int32 Tmp = static_cast<Int32>(std::to_underlying(Value));
        WriteNumber(Tmp);
    }
};

class InputArchive
{
public:
    virtual ~InputArchive() = default;

    virtual void ReadArraySize(Int64& OutArraySize) = 0;
    virtual void ReadMapSize(Int64& OutMapSize) = 0;

    virtual void ReadNumber(StringView InName, Int8& OutData) = 0;
    virtual void ReadNumber(StringView InName, Int16& OutData) = 0;
    virtual void ReadNumber(StringView InName, Int32& OutData) = 0;
    virtual void ReadNumber(StringView InName, Int64& OutData) = 0;
    virtual void ReadNumber(StringView InName, UInt8& OutData) = 0;
    virtual void ReadNumber(StringView InName, UInt16& OutData) = 0;
    virtual void ReadNumber(StringView InName, UInt32& OutData) = 0;
    virtual void ReadNumber(StringView InName, UInt64& OutData) = 0;
    virtual void ReadNumber(StringView InName, Float& OutData) = 0;
    virtual void ReadNumber(StringView InName, Double& OutData) = 0;
    virtual void ReadNumber(Int8& OutData) = 0;
    virtual void ReadNumber(Int16& OutData) = 0;
    virtual void ReadNumber(Int32& OutData) = 0;
    virtual void ReadNumber(Int64& OutData) = 0;
    virtual void ReadNumber(UInt8& OutData) = 0;
    virtual void ReadNumber(UInt16& OutData) = 0;
    virtual void ReadNumber(UInt32& OutData) = 0;
    virtual void ReadNumber(UInt64& OutData) = 0;
    virtual void ReadNumber(Float& OutData) = 0;
    virtual void ReadNumber(Double& OutData) = 0;

    virtual void ReadBool(StringView InName, bool& OutData) = 0;
    virtual void ReadBool(bool& OutData) = 0;

    virtual void ReadString(StringView InName, String& OutData) = 0;
    virtual void ReadString(String& OutData) = 0;

    virtual void SetNextScopeName(StringView InName) = 0;
    virtual void BeginScope() = 0;
    virtual void EndScope() = 0;

    template <typename T>
        requires(!Traits::IsEnum<T>)
    ELBOW_FORCE_INLINE void ReadType(const StringView InName, T& Value)
    {
        if constexpr (Traits::IsAnyOf<Pure<T>, Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64, Float, Double>)
        {
            ReadNumber(InName, Value);
        }
        else if constexpr (Traits::IsAnyOf<Pure<T>, bool>)
        {
            ReadBool(InName, Value);
        }
        else if constexpr (Traits::IsAnyOf<Pure<T>, String>)
        {
            ReadString(InName, Value);
        }
        else
        {
            SetNextScopeName(InName);
            BeginScope();
            Value.Serialization_Load(*this);
            EndScope();
        }
    }

    template <typename T>
        requires(!Traits::IsEnum<T>)
    ELBOW_FORCE_INLINE void ReadType(T& Value)
    {
        if constexpr (Traits::IsAnyOf<Pure<T>, Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64, Float, Double>)
        {
            ReadNumber(Value);
        }
        else if constexpr (Traits::IsAnyOf<Pure<T>, bool>)
        {
            ReadBool(Value);
        }
        else if constexpr (Traits::IsAnyOf<Pure<T>, String>)
        {
            ReadString(Value);
        }
        else
        {
            BeginScope();
            Value.Serialization_Load(*this);
            EndScope();
        }
    }

    template <typename T>
        requires(Traits::IsEnum<T>)
    ELBOW_FORCE_INLINE void ReadType(const StringView InName, T& Value)
    {
        Int32 Tmp;
        ReadNumber(InName, Tmp);
        Value = Tmp;
    }

    template <typename T>
        requires(Traits::IsEnum<T>)
    ELBOW_FORCE_INLINE void ReadType(T& Value)
    {
        Int32 Tmp;
        ReadNumber(Tmp);
        Value = Tmp;
    }
};
