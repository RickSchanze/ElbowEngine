#pragma once
#include "Archive.hpp"
#include "cereal/archives/xml.hpp"
#include "Core/TypeTraits.hpp"
#include "Core/Object/Object.hpp"
namespace cereal
{
// 适配cereal的序列化
template<typename TType>
void save(::cereal::XMLOutputArchive& Archive, const TType& Value);

template<typename TType>
void load(::cereal::XMLInputArchive& Archive, TType& Value);
}

class XMLOutputArchive : public OutputArchive
{
public:
    // 适配cereal的序列化
    template<typename TType>
    friend void cereal::save(::cereal::XMLOutputArchive& Archive, const TType& Value);

    /**
     * 构造一个XML输出流
     * @param InOutputStream 此流可以是文件流、字符串流，etc...
     */
    explicit XMLOutputArchive(std::ostream& InOutputStream);


    virtual ~XMLOutputArchive() override;

    virtual void WriteArraySize(UInt64 InSize) override;
    virtual void WriteMapSize(UInt64 InSize) override;

    virtual void WriteNumber(StringView InName, Int8& Data) override;
    virtual void WriteNumber(StringView InName, Int16& Data) override;
    virtual void WriteNumber(StringView InName, Int32& Data) override;
    virtual void WriteNumber(StringView InName, Int64& Data) override;
    virtual void WriteNumber(StringView InName, UInt8& Data) override;
    virtual void WriteNumber(StringView InName, UInt16& Data) override;
    virtual void WriteNumber(StringView InName, UInt32& Data) override;
    virtual void WriteNumber(StringView InName, UInt64& Data) override;
    virtual void WriteNumber(StringView InName, Float& Data) override;
    virtual void WriteNumber(StringView InName, Double& Data) override;
    virtual void WriteNumber(Int8& Data) override;
    virtual void WriteNumber(Int16& Data) override;
    virtual void WriteNumber(Int32& Data) override;
    virtual void WriteNumber(Int64& Data) override;
    virtual void WriteNumber(UInt8& Data) override;
    virtual void WriteNumber(UInt16& Data) override;
    virtual void WriteNumber(UInt32& Data) override;
    virtual void WriteNumber(UInt64& Data) override;
    virtual void WriteNumber(Float& Data) override;
    virtual void WriteNumber(Double& Data) override;

    virtual void WriteBool(StringView InName, bool& Data) override;
    virtual void WriteBool(bool& Data) override;

    virtual void WriteString(StringView InName, String& Data) override;
    virtual void WriteString(String& Data) override;

    void WriteObject(Object* InObj)
    {
        if (InObj)
        {
            WriteObject(*InObj);
        }
        VLOG_ERROR("序列化Obj失败! InObj是空的!");
    }

    void WriteObject(Object& InObj);

    virtual void SetNextScopeName(StringView InName) override;
    virtual void BeginScope() override;
    virtual void EndScope() override;

private:
    /**
     * 使用已存在的Archive初始化, 主要是为了配合cereal的序列化流程
     * 因此你在外部使用时**不应该**调用构造函数
     * @param InExistArchive
     */
    explicit XMLOutputArchive(cereal::XMLOutputArchive* InExistArchive);


private:
    cereal::XMLOutputArchive* mArchive;
    bool mRequireDelete = true;
};


class XMLInputArchive : public InputArchive
{
public:
    template<typename TType>
    friend void ::cereal::load(::cereal::XMLInputArchive& Archive, TType& Value);

    /**
     * 构造一个XML输入流
     * @param InInputStream 此流可以是文件流、字符串流，etc...
     */
    explicit XMLInputArchive(std::istream& InInputStream);


    virtual ~XMLInputArchive() override;

    virtual void ReadArraySize(Int64& OutArraySize) override;
    virtual void ReadMapSize(Int64& OutMapSize) override;

    virtual void ReadNumber(StringView InName, Int8& OutData) override;
    virtual void ReadNumber(StringView InName, Int16& OutData) override;
    virtual void ReadNumber(StringView InName, Int32& OutData) override;
    virtual void ReadNumber(StringView InName, Int64& OutData) override;
    virtual void ReadNumber(StringView InName, UInt8& OutData) override;
    virtual void ReadNumber(StringView InName, UInt16& OutData) override;
    virtual void ReadNumber(StringView InName, UInt32& OutData) override;
    virtual void ReadNumber(StringView InName, UInt64& OutData) override;
    virtual void ReadNumber(StringView InName, Float& OutData) override;
    virtual void ReadNumber(StringView InName, Double& OutData) override;
    virtual void ReadNumber(Int8& OutData) override;
    virtual void ReadNumber(Int16& OutData) override;
    virtual void ReadNumber(Int32& OutData) override;
    virtual void ReadNumber(Int64& OutData) override;
    virtual void ReadNumber(UInt8& OutData) override;
    virtual void ReadNumber(UInt16& OutData) override;
    virtual void ReadNumber(UInt32& OutData) override;
    virtual void ReadNumber(UInt64& OutData) override;
    virtual void ReadNumber(Float& OutData) override;
    virtual void ReadNumber(Double& OutData) override;

    virtual void ReadBool(StringView InName, bool& OutData) override;
    virtual void ReadBool(bool& OutData) override;

    virtual void ReadString(StringView InName, String& OutData) override;
    virtual void ReadString(String& OutData) override;

private:
    /**
         * 使用已存在的Archive初始化, 主要是为了配合cereal的序列化流程
         * 因此你在外部使用时**不应该**调用构造函数
         * @param InExistArchive
         */
    explicit XMLInputArchive(cereal::XMLInputArchive* InExistArchive);

public:
    virtual void SetNextScopeName(StringView InName) override;
    virtual void BeginScope() override;
    virtual void EndScope() override;

private:
    cereal::XMLInputArchive* mArchive;
    bool mRequireDelete = true;
};

namespace cereal
{
// 适配cereal的序列化
template<typename TType>
void save(::cereal::XMLOutputArchive& Archive, const TType& Value)
{
    ::XMLOutputArchive ExistArchive(&Archive);
    Value.Serialization_Save(ExistArchive);
}

template<typename TType>
void load(::cereal::XMLInputArchive& Archive, TType& Value)
{
    ::XMLInputArchive ExistArchive(&Archive);
    Value.Serialization_Load(ExistArchive);
}
}
