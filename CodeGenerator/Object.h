/**
 * @file Object.h
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#ifndef ELBOWENGINE_OBJECT_H
#define ELBOWENGINE_OBJECT_H
#include "rttr/registration"
#include "rttr/policy.h"



#define RTTR_REGISTRATION_IN_CLASS(name)                                      \
    struct z_rttr__auto__register__##name {                                   \
        z_rttr__auto__register__##name() {                                    \
            zrttr_auto_register_reflection_function_##name();                 \
        }                                                                     \
    };                                                                        \
    static inline const z_rttr__auto__register__##name auto_register__##name; \
    static void zrttr_auto_register_reflection_function_##name()


class Object {
public:
    RTTR_ENABLE()
};

RTTR_REGISTRATION{
        rttr::registration::class_<Object>("Object");
};


#endif//ELBOWENGINE_OBJECT_H
