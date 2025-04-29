//
// Created by kita on 25-4-27.
//

#include "XMLArchive.hpp"

#include <fstream>

#include "cereal/types/map.hpp"
#include "Core/FileSystem/File.hpp"

XMLOutputArchive::XMLOutputArchive(std::ostream& InOutputStream)
{
    mArchive = New<cereal::XMLOutputArchive>(InOutputStream);
}

XMLOutputArchive::XMLOutputArchive(cereal::XMLOutputArchive* InExistArchive)
{
    Assert(InExistArchive, "输入Archive必须存在");
    mArchive = InExistArchive;
    mRequireDelete = false;
}

XMLOutputArchive::~XMLOutputArchive()
{
    if (mRequireDelete)
        Delete(mArchive);
}

void XMLOutputArchive::WriteArraySize(const UInt64 InSize)
{
    (*mArchive)(cereal::make_size_tag(InSize));
}

void XMLOutputArchive::WriteMapSize(const UInt64 InSize)
{
    (*mArchive)(cereal::make_size_tag(InSize));
}

void XMLOutputArchive::WriteNumber(const StringView InName, const Int8& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const StringView InName, const Int16& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const StringView InName, const Int32& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const StringView InName, const Int64& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const StringView InName, const UInt8& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const StringView InName,const  UInt16& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const StringView InName, const UInt32& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const StringView InName, const UInt64& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const StringView InName, const Float& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const StringView InName, const Double& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(const Int8& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(const Int16& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(const Int32& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(const Int64& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(const UInt8& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(const UInt16& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(const UInt32& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(const UInt64& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(const Float& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(const Double& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteBool(const StringView InName, const bool& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteBool(const bool& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteString(const StringView InName, const String& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data.ToStdString()));
}

void XMLOutputArchive::WriteString(const String& Data)
{
    (*mArchive)(Data.ToStdString());
}

void XMLOutputArchive::SetNextScopeName(const StringView InName)
{
    mArchive->setNextName(*InName);
}

void XMLOutputArchive::BeginScope()
{
    mArchive->startNode();
}

void XMLOutputArchive::EndScope()
{
    mArchive->finishNode();
}

XMLInputArchive::XMLInputArchive(std::istream& InInputStream)
{
    mArchive = New<cereal::XMLInputArchive>(InInputStream);
}

XMLInputArchive::XMLInputArchive(const StringView InPath)
{
    if (File::IsExist(InPath))
    {
        mArchive = New<cereal::XMLInputArchive>(std::ifstream(*InPath));
    }
}

XMLInputArchive::XMLInputArchive(cereal::XMLInputArchive* InExistArchive)
{
    Assert(InExistArchive, "输入的Archive必须存在");
    mArchive = InExistArchive;
    mRequireDelete = false;
}

void XMLInputArchive::SetNextScopeName(const StringView InName)
{
    mArchive->setNextName(*InName);
}

void XMLInputArchive::BeginScope()
{
    mArchive->startNode();
}

void XMLInputArchive::EndScope()
{
    mArchive->finishNode();
}

void XMLInputArchive::Deserialize(const Object& InObjectRef) {}

XMLInputArchive::~XMLInputArchive()
{
    if (mRequireDelete)
        Delete(mArchive);
}

void XMLInputArchive::ReadArraySize(Int64& OutArraySize)
{
    (*mArchive)(cereal::make_size_tag(OutArraySize));
}

void XMLInputArchive::ReadMapSize(Int64& OutMapSize)
{
    (*mArchive)(cereal::make_size_tag(OutMapSize));
}

void XMLInputArchive::ReadNumber(const StringView InName, Int8& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(const StringView InName, Int16& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(const StringView InName, Int32& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(const StringView InName, Int64& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(const StringView InName, UInt8& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(const StringView InName, UInt16& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(const StringView InName, UInt32& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(const StringView InName, UInt64& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(const StringView InName, Float& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(const StringView InName, Double& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(Int8& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadNumber(Int16& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadNumber(Int32& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadNumber(Int64& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadNumber(UInt8& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadNumber(UInt16& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadNumber(UInt32& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadNumber(UInt64& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadNumber(Float& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadNumber(Double& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadBool(const StringView InName, bool& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadBool(bool& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadString(const StringView InName, String& OutData)
{
    std::string Out;
    (*mArchive)(cereal::make_nvp(*InName, Out));
    OutData = String(Out);
}

void XMLInputArchive::ReadString(String& OutData)
{
    std::string Out;
    (*mArchive)(Out);
    OutData = String(Out);
}
