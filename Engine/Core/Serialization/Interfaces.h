#pragma once

#include "CoreDef.h"
#include "Exception/Exception.h"
#include "Utils/StringUtils.h"
#include "yaml-cpp/yaml.h"

#include <format>

interface ISerializer {
public:
    virtual ~ISerializer() = default;

    /**
     * 将Obj序列化为到Stream中
     * @param Obj 要序列化的对象
     * @param Stream 输出流
     * @return
     */
    virtual bool Serialize(const rttr::instance& Obj, AnsiOutputStream& Stream) INTERFACE_METHOD;

    /**
     * 自动打开一个文件并将Obj序列化到文件中
     * @param Obj 要序列化的对象
     * @param Filename 文件名
     * @note 打开失败抛出异常
     * @return
     */
    virtual bool SerializeFile(const rttr::instance& Obj, StringView Filename);

    /**
     * 将Obj序列化到AnsiString中
     * @param Obj 序列化对象
     * @param Str 输出的Str
     * @return
     */
    virtual bool Serialize(const rttr::instance& Obj, OUT AnsiString& Str);

    /**
     * 将Obj序列化到String中
     * @param Obj
     * @param OutStr 
     * @return 
     */
    virtual bool Serialize(const rttr::instance& Obj, OUT String& OutStr);
};

interface IDeserializer {
public:
    virtual ~IDeserializer() = default;

    /**
     * 从流反序列化一个对象
     * @param Stream
     * @param Obj
     * @param
     * @return
     */
    virtual bool Deserialize(AnsiInputStream & Stream, OUT rttr::instance &Obj) INTERFACE_METHOD;

    /**
     * 从AnsiStringView反序列化一个对象
     * @param StrView
     * @param Obj
     * @return
     */
    virtual bool Deserialize(AnsiStringView StrView, OUT rttr::instance & Obj);

    /**
     * 从StringView反序列化一个对象
     * @param StrView
     * @param Obj
     * @note 由于StringView到AnsiStringView的转换，可能会有性能损失
     * 转换到AnsiStringView是因为最终都是调用接受AnsiInputStream的重载
     * @return
     */
    virtual bool Deserialize(StringView StrView, OUT rttr::instance& Obj);

    /**
     * 从文件反序列化一个对象
     * @param Filename
     * @param Obj
     * @return
     */
    virtual bool DeserializeFile(StringView Filename, OUT rttr::instance& Obj) ;
};

inline bool ISerializer::SerializeFile(const rttr::instance& Obj, const StringView Filename) {
    FileOutputStream Stream;
    Stream.open(Filename.data(), std::ios::out);
    if (!Stream.is_open()) {
        throw FileOpenException(Filename.data(), errno);
    }
    return Serialize(Obj, Stream);
}

inline bool ISerializer::Serialize(const rttr::instance& Obj, AnsiString& Str) {
    std::stringstream Stream;
    if (!Serialize(Obj, Stream)) {
        Str = "Serialize failed!";
        return false;
    }
    Str = Stream.str();
    return true;
}

inline bool ISerializer::Serialize(const rttr::instance& Obj, String& OutStr) {
    AnsiString String;
    if (!Serialize(Obj, String)) {
        OutStr = L"序列化失败！";
        return false;
    }
    OutStr = StringUtils::FromAnsiString(String);
    return true;
}

inline bool IDeserializer::Deserialize(const AnsiStringView StrView, rttr::instance& Obj) {
    AnsiStringStream Stream;
    Stream << StrView;
    return Deserialize(Stream, Obj);
}

inline bool IDeserializer::Deserialize(const StringView StrView, rttr::instance& Obj) {
    StringStream Stream;
    Stream << StrView;
    const AnsiString AnsiStreamStr = StringUtils::ToAnsiString(Stream.str());
    return Deserialize(AnsiStreamStr, Obj);
}

inline bool IDeserializer::DeserializeFile(const StringView Filename, rttr::instance& Obj){
    FileInputStream Stream;
    Stream.open(Filename.data(), std::ios::in);
    if (!Stream.is_open()) {
        throw FileOpenException(Filename.data(), errno);
    }
    return Deserialize(Stream, Obj);
}
