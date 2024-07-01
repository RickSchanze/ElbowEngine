/**
 * @file Guid.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "Guid.h"

#include "Utils/StringUtils.h"

Guid::Guid() {
    CoCreateGuid(&guid_);
}

AnsiString Guid::ToAnsiString() const {
    char buffer[length] = {0};
    sprintf_s(
        buffer,
        "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        guid_.Data1,
        guid_.Data2,
        guid_.Data3,
        guid_.Data4[0],
        guid_.Data4[1],
        guid_.Data4[2],
        guid_.Data4[3],
        guid_.Data4[4],
        guid_.Data4[5],
        guid_.Data4[6],
        guid_.Data4[7]
    );
    return {buffer};
}

String Guid::ToString() const {
    char buffer[length] = {0};
    sprintf_s(
        buffer,
        "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        guid_.Data1,
        guid_.Data2,
        guid_.Data3,
        guid_.Data4[0],
        guid_.Data4[1],
        guid_.Data4[2],
        guid_.Data4[3],
        guid_.Data4[4],
        guid_.Data4[5],
        guid_.Data4[6],
        guid_.Data4[7]
    );
    return StringUtils::FromAnsiString(buffer);
}
