#pragma once
#include "Archive.hpp"
#include "Core/Object/Object.hpp"
#include "Core/TypeTraits.hpp"
#include "cereal/archives/xml.hpp"

class XMLOutputArchive : public OutputArchive
{
public:
    /**
     * 构造一个XML输出流
     * @param InOutputStream 此流可以是文件流、字符串流，etc...
     */
    explicit XMLOutputArchive(std::ostream& InOutputStream);

    virtual ~XMLOutputArchive() override;

    virtual void WriteArraySize(UInt64 InSize) override;
    virtual void WriteMapSize(UInt64 InSize) override;

    virtual void WriteNumber(StringView InName, const Int8& Data) override;
    virtual void WriteNumber(StringView InName, const Int16& Data) override;
    virtual void WriteNumber(StringView InName, const Int32& Data) override;
    virtual void WriteNumber(StringView InName, const Int64& Data) override;
    virtual void WriteNumber(StringView InName, const UInt8& Data) override;
    virtual void WriteNumber(StringView InName, const UInt16& Data) override;
    virtual void WriteNumber(StringView InName, const UInt32& Data) override;
    virtual void WriteNumber(StringView InName, const UInt64& Data) override;
    virtual void WriteNumber(StringView InName, const Float& Data) override;
    virtual void WriteNumber(StringView InName, const Double& Data) override;
    virtual void WriteNumber(const Int8& Data) override;
    virtual void WriteNumber(const Int16& Data) override;
    virtual void WriteNumber(const Int32& Data) override;
    virtual void WriteNumber(const Int64& Data) override;
    virtual void WriteNumber(const UInt8& Data) override;
    virtual void WriteNumber(const UInt16& Data) override;
    virtual void WriteNumber(const UInt32& Data) override;
    virtual void WriteNumber(const UInt64& Data) override;
    virtual void WriteNumber(const Float& Data) override;
    virtual void WriteNumber(const Double& Data) override;

    virtual void WriteBool(StringView InName, const bool& Data) override;
    virtual void WriteBool(const bool& Data) override;

    virtual void WriteString(StringView InName, const String& Data) override;
    virtual void WriteString(const String& Data) override;

    template <typename T>
    ELBOW_FORCE_INLINE void Serialize(T& InValue)
    {
        if constexpr (CHas_Serialization_BeforeSave<T>)
        {
            InValue.Serialization_BeforeSave();
        }
        // 先写入类型元信息
        SetNextScopeName("TypeMeta");
        BeginScope();
        const Type* MyType = TypeOf<T>();
        const String TypeName = MyType->GetName();
        const UInt64 TypeHash = MyType->GetHashCode();
        WriteString("TypeName", TypeName);
        WriteNumber("TypeHash", TypeHash);
        EndScope();
        // 写入实际数据
        WriteType("Data", InValue);
        if constexpr (CHas_Serialization_AfterSave<T>)
        {
            InValue.Serialization_AfterSave();
        }
    }

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

    template <typename T>
    ELBOW_FORCE_INLINE void Deserialize(T& InValue)
    {
        if constexpr (CHas_Serialization_BeforeLoad<T>)
        {
            InValue.Serialization_BeforeLoad();
        }
        // 先写入类型元信息
        SetNextScopeName("TypeMeta");
        BeginScope();
        const Type* MyType = TypeOf<T>();
        String TypeName;
        UInt64 TypeHash;
        ReadString("TypeName", TypeName);
        ReadNumber("TypeHash", TypeHash);
        EndScope();
        if (MyType->GetName() != TypeName || MyType->GetHashCode() != TypeHash)
        {
            VLOG_FATAL("反序列化错误, 类型元数据校验失败.");
        }
        // 写入实际数据
        ReadType("Data", InValue);
        if constexpr (CHas_Serialization_AfterLoad<T>)
        {
            InValue.Serialization_AfterLoad();
        }
    }

private:
    cereal::XMLInputArchive* mArchive;
    bool mRequireDelete = true;
};
