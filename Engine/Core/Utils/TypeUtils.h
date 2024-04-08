/**
 * @file TypeUtils.h
 * @author Echo 
 * @Date 24-4-8
 * @brief 
 */

#ifndef TYPEUTILS_H
#define TYPEUTILS_H
#include "Core/CoreDef.h"



namespace rttr {
class property;
}
class TypeUtils {

};

class PropUtils
{
public:
    static bool HasAttr(const rttr::property& Property, const String& AttrName);
};



#endif //TYPEUTILS_H
