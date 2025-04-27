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

void XMLOutputArchive::WriteArraySize(UInt64 InSize)
{
    (*mArchive)(cereal::make_size_tag(InSize));
}

void XMLOutputArchive::WriteMapSize(UInt64 InSize)
{
    (*mArchive)(cereal::make_size_tag(InSize));
}

void XMLOutputArchive::WriteNumber(StringView InName, Int8& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(StringView InName, Int16& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(StringView InName, Int32& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(StringView InName, Int64& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(StringView InName, UInt8& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(StringView InName, UInt16& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(StringView InName, UInt32& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(StringView InName, UInt64& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(StringView InName, Float& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(StringView InName, Double& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteNumber(Int8& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(Int16& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(Int32& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(Int64& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(UInt8& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(UInt16& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(UInt32& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(UInt64& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(Float& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteNumber(Double& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteBool(StringView InName, bool& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data));
}

void XMLOutputArchive::WriteBool(bool& Data)
{
    (*mArchive)(Data);
}

void XMLOutputArchive::WriteString(StringView InName, String& Data)
{
    (*mArchive)(cereal::make_nvp(*InName, Data.ToStdString()));
}

void XMLOutputArchive::WriteString(String& Data)
{
    (*mArchive)(Data.ToStdString());
}

void XMLOutputArchive::WriteObject(Object& InObj) {}

void XMLOutputArchive::SetNextScopeName(StringView InName)
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

XMLInputArchive::XMLInputArchive(StringView InPath)
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

void XMLInputArchive::SetNextScopeName(StringView InName)
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

void XMLInputArchive::ReadNumber(StringView InName, Int8& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(StringView InName, Int16& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(StringView InName, Int32& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(StringView InName, Int64& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(StringView InName, UInt8& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(StringView InName, UInt16& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(StringView InName, UInt32& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(StringView InName, UInt64& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(StringView InName, Float& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadNumber(StringView InName, Double& OutData)
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

void XMLInputArchive::ReadBool(StringView InName, bool& OutData)
{
    (*mArchive)(cereal::make_nvp(*InName, OutData));
}

void XMLInputArchive::ReadBool(bool& OutData)
{
    (*mArchive)(OutData);
}

void XMLInputArchive::ReadString(StringView InName, String& OutData)
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
