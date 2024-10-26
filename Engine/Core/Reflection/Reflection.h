/**
 * @file Reflection.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once
#include "Base/CoreTypeDef.h"
#include "Base/EString.h"

namespace core
{

struct Type;

struct FiledInfo
{
    int32_t offset;
    String  name;
    Type*   type = nullptr;
};

struct Type
{
public:
    [[nodiscard]] const Array<FiledInfo>& GetFields() const { return fields; }

    [[nodiscard]] const FiledInfo& GetField(int32_t index) const { return fields[index]; }

    [[nodiscard]] const FiledInfo& GetField(StringView view) const;

private:
    Array<FiledInfo> fields;
};

}   // namespace core
