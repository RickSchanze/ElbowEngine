/**
 * @file Object.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#ifndef ELBOWENGINE_OBJECT_H
#define ELBOWENGINE_OBJECT_H
#include "Core/CoreDef.h"
#include "Object.generated.h"

class Object {
    RTTR_ENABLE()
    RTTR_REGISTRATION_FRIEND
    REGISTRATION_IN_CLASS(Object_h, Object) {
        rttr::registration::class_<Object>("Object").property("Name", &Object::mName);
    }

public:
    virtual ~Object() = default;

    /**
     * 获取反射类型
     * @return rttr::typr
     */
    virtual rttr::type GetType() const { return get_type(); }

public:
    /**
     * 获取对象名字
     * @return
     */
    [[nodiscard]] String GetName() const { return mName; }

    /**
     * 设置对象的名字
     * @param Name
     */
    void SetName(String Name) { mName = std::move(Name); }

    void TestYaml();

private:
    String mName;
};

class REFL TestRefl : public Object {
    GENERATED_BODY(TestRefl)

private:
    PROPERTY(Name = "Test")
    int32 C;
};


#endif   //ELBOWENGINE_OBJECT_H
