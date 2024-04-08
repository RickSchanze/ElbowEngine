#ifndef CORE_SERIALIZATION_ISERIALIZER_H
#define CORE_SERIALIZATION_ISERIALIZER_H

#include "Core/CoreDef.h"
#include "Core/Utils/StringUtils.h"
#include "yaml-cpp/yaml.h"

interface ISerializer {
public:
    virtual ~ISerializer()                                                    = default;
    virtual bool Serialize(const rttr::instance& Obj, OUT AnsiString& OutStr) = 0;
    virtual bool Serialize(const rttr::instance& Obj, OUT String& OutStr);
};

inline bool ISerializer::Serialize(const rttr::instance& Obj, String& OutStr) {
    AnsiString String;
    if (!Serialize(Obj, String)) {
        OutStr = L"序列化失败！";
        return false;
    }
    OutStr = StringUtils::FromStdString(String);
    return true;
}

#endif
