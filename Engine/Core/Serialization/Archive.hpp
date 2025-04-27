//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/TypeTraits.hpp"
#include "Core/String/String.hpp"

class OutputArchive
{
public:
    virtual ~OutputArchive() = default;

    virtual void WriteArraySize(UInt64 InSize) = 0;
    virtual void WriteMapSize(UInt64 InSize) = 0;

    virtual void WriteNumber(StringView InName, Int8& Data) = 0;
    virtual void WriteNumber(StringView InName, Int16& Data) = 0;
    virtual void WriteNumber(StringView InName, Int32& Data) = 0;
    virtual void WriteNumber(StringView InName, Int64& Data) = 0;
    virtual void WriteNumber(StringView InName, UInt8& Data) = 0;
    virtual void WriteNumber(StringView InName, UInt16& Data) = 0;
    virtual void WriteNumber(StringView InName, UInt32& Data) = 0;
    virtual void WriteNumber(StringView InName, UInt64& Data) = 0;
    virtual void WriteNumber(StringView InName, Float& Data) = 0;
    virtual void WriteNumber(StringView InName, Double& Data) = 0;

    virtual void WriteNumber(Int8& Data) = 0;
    virtual void WriteNumber(Int16& Data) = 0;
    virtual void WriteNumber(Int32& Data) = 0;
    virtual void WriteNumber(Int64& Data) = 0;
    virtual void WriteNumber(UInt8& Data) = 0;
    virtual void WriteNumber(UInt16& Data) = 0;
    virtual void WriteNumber(UInt32& Data) = 0;
    virtual void WriteNumber(UInt64& Data) = 0;
    virtual void WriteNumber(Float& Data) = 0;
    virtual void WriteNumber(Double& Data) = 0;

    virtual void WriteBool(StringView InName, bool& Data) = 0;
    virtual void WriteBool(bool& Data) = 0;

    // 不允许StringView序列化
    virtual void WriteString(StringView InName, String& Data) = 0;
    virtual void WriteString(String& Data) = 0;

    virtual void SetNextScopeName(StringView InName) = 0;
    virtual void BeginScope() = 0;
    virtual void EndScope() = 0;

    template<typename T> requires (!IsEnum<T>)
    void WriteType(StringView InName, const T& Value)
    {
        SetNextScopeName(InName);
        BeginScope();
        Value.Serialization_Save(*this);
        EndScope();
    }

    template<typename T> requires (!IsEnum<T>)
    void WriteType(const T& Value)
    {
        BeginScope();
        Value.Serialization_Save(*this);
        EndScope();
    }

    template<typename T> requires (IsEnum<T>)
    void WriteType(StringView InName, const T& Value)
    {
        Int32 Tmp = static_cast<Int32>(std::to_underlying(Value));
        WriteNumber(InName, Tmp);
    }

    template<typename T> requires (IsEnum<T>)
    void WriteType(const T& Value)
    {
        Int32 Tmp = static_cast<Int32>(std::to_underlying(Value));
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
    virtual void ReadNumber(StringView InName, Int64& OutData) =0;
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

    template<typename T> requires (!IsEnum<T>)
    void ReadType(StringView InName, T& Value)
    {
        SetNextScopeName(InName);
        BeginScope();
        Value.Serialization_Load(*this);
        EndScope();
    }

    template<typename T> requires (!IsEnum<T>)
    void ReadType(T& Value)
    {
        BeginScope();
        Value.Serialization_Save(*this);
        EndScope();
    }

    template<typename T> requires (IsEnum<T>)
    void ReadType(StringView InName, T& Value)
    {
        Int32 Tmp;
        ReadNumber(InName, Tmp);
        Value = Tmp;
    }

    template<typename T> requires (IsEnum<T>)
    void ReadType(T& Value)
    {
        Int32 Tmp;
        ReadNumber(Tmp);
        Value = Tmp;
    }
};
