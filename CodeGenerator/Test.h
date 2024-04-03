/**
 * @file Test.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#ifndef ELBOWENGINE_TEST_H
#define ELBOWENGINE_TEST_H
#include "Object.h"
#include "Test.generated.h"

#define PROPERTY(...) __attribute__((annotate("Reflected, " #__VA_ARGS__)))
#define REFLECTED __attribute__((annotate("Reflected, ")))
#define Serialized Serialized
//#define PROPERTY(...)
//#define REFLECTED
#define BODY_MACRO_COMBINE_INNER(A, B, C, D) A##B##C##D
#define BODY_MACRO_COMBINE(A, B, C, D) BODY_MACRO_COMBINE_INNER(A, B, C, D)
#define GENERATED_BODY(...) BODY_MACRO_COMBINE(GENERATED_BODY_INTERNAL_, CURRENT_FILE_ID, _, __LINE__)
#define REFLECTED_API REFLECTED __declspec(dllexport)

class REFLECTED_API A : public Object {
    GENERATED_BODY()

public:
    PROPERTY(K)
    int d = 20;

    PROPERTY(Serialized, Name = APC)
    int ABC{};
};

class REFLECTED C : public Object {
    GENERATED_BODY()

private:

    PROPERTY(K)
    int d;

    PROPERTY(Serialized, Name = APC)
    int ABC;

    PROPERTY(F)
    std::vector<A> a = {A{}};
};

//RTTR_REGISTRATION {
//    rttr::registration::class_<A>("A")
//            .property("b", &A::b);
//};

#endif//ELBOWENGINE_TEST_H
