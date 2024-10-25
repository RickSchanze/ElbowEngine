/**
 * @file EString.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#include "EString.h"
#include "CoreGlobal.h"
#include "unicode/unistr.h"

String::String(const StringView& view) : str_(view.Data(), view.Length()) {}

int32_t String::Size() const
{
    const icu::UnicodeString str(this->str_.c_str());
    return str.length();
}

int32_t String::Length() const
{
    return static_cast<int32_t>(str_.size());
}

void StringView::RemoveSubfix(int c)
{
    // DebugAssert
    length_ -= c;
}
