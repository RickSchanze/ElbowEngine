/**
 * @file Guid.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "Guid.h"

#include "Utils/StringUtils.h"

Guid::Guid() {
    CoCreateGuid(&mGuid);
}

AnsiString Guid::ToAnsiString() const {
    char buffer[mGuidLength] = {0};
    sprintf_s(
        buffer,
        "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        mGuid.Data1,
        mGuid.Data2,
        mGuid.Data3,
        mGuid.Data4[0],
        mGuid.Data4[1],
        mGuid.Data4[2],
        mGuid.Data4[3],
        mGuid.Data4[4],
        mGuid.Data4[5],
        mGuid.Data4[6],
        mGuid.Data4[7]
    );
    return {buffer};
}

String Guid::ToString() const {
    char buffer[mGuidLength] = {0};
    sprintf_s(
        buffer,
        "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        mGuid.Data1,
        mGuid.Data2,
        mGuid.Data3,
        mGuid.Data4[0],
        mGuid.Data4[1],
        mGuid.Data4[2],
        mGuid.Data4[3],
        mGuid.Data4[4],
        mGuid.Data4[5],
        mGuid.Data4[6],
        mGuid.Data4[7]
    );
    return StringUtils::FromAnsiString(buffer);
}
