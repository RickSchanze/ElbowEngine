#ifdef TEST_GENERATED_H
#error "generated file for Test.h has been included!"
#endif

#ifndef TEST_GENERATED_H
#define TEST_GENERATED_H


#define GENERATED_CODE_A                   \
    {                                      \
        rttr::registration::class_<A>("A") \
                .property("b", &A::b);     \
    }

#define GENERATED_BODY_INTERNAL_A \
public:                           \
    typedef Object Super;         \
    typedef A ThisClass;          \
    RTTR_REGISTRATION_IN_CLASS(A) \
    GENERATED_CODE_A              \
    RTTR_ENABLE(Object)           \
private:

#endif
