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
class ReflUtils {
public:
    /**
     * 查看Property名为AttrName属性的值
     * @param Property
     * @param AttrName
     * @return 若不存在此属性或Property不存在则返回值为空
     */
    static AnsiString GetPropertyAttribute(const rttr::property& Property, const AnsiString& AttrName);

    /**
     * 看一个Property AttrName属性的值是否为"True"
     * @param Property
     * @param AttrName
     * @return
     */
    static bool CheckAttribute(const rttr::property& Property, const AnsiString& AttrName);

    /**
     * 这个Property可序列化吗？
     * @param Property
     * @return
     */
    static bool IsPropertySerializable(const rttr::property& Property);

    /**
     * 这个Property序列化为引用吗？
     * @param Property
     * @return
     */
    static bool IsPropertySerializedByReference(const rttr::property& Property);
};



#endif //TYPEUTILS_H
