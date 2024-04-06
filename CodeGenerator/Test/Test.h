/**
 * @file Test.h
 * @author Echo 
 * @Date 24-4-5
 * @brief 
 */

#ifndef CODEGENERATOR_TEST_H
#define CODEGENERATOR_TEST_H
#include "Object.h"
#include "Test.generated.h"

enum class REFL TestEnum1 {
    A,
    PV,
};
GENERATED_ENUM(TestEnum1)


class REFL T2 : public Object {
    GENERATED_BODY(T2)
private:
    PROPERTY()
    int c;
};

class REFL T1 : public Object {
    GENERATED_BODY(T1)
};


namespace Test {
class REFL TestReflection : public Object {
    GENERATED_BODY(TestReflection)
private:
    PROPERTY(Name = K, Serialized)
    int32 i32;

    PROPERTY(Name = K1, Serialized)
    int64 i64;

    PROPERTY()
    Array<int8> CharArray;
};
}   // namespace Test


enum class REFL TestEnum : char { A, B, C };
GENERATED_ENUM(TestEnum)


enum class REFL TestEnumc : char { A, B, C };
GENERATED_ENUM(TestEnumc)

#include "rttr/registration"

#endif   //CODEGENERATOR_TEST_H
