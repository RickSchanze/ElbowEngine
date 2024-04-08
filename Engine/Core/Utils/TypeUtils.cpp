/**
 * @file TypeUtils.cpp
 * @author Echo 
 * @Date 24-4-8
 * @brief 
 */

#include "TypeUtils.h"
bool PropUtils::HasAttr(const rttr::property& Property, const String& AttrName) {
    const auto AttrValue = Property.get_metadata(AttrName).get_value<String>();
    return AttrValue == L"True";
}
