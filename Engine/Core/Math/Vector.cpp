//
// Created by kita on 25-4-27.
//

#include "Vector.hpp"
#define REGISTER_MATH_TYPE(name, type_register)                                                                                                      \
    {                                                                                                                                                \
        auto type_info = RTTITypeInfo::Create<name>(#name);                                                                                          \
        Mgr.RegisterCtorDtor(type_info, &name::ConstructSelf, &name::DestructSelf);                                                                  \
        Mgr.RegisterTypeRegisterer(type_info, type_register);                                                                                        \
    }

MathTypeRegTrigger_Vector::MathTypeRegTrigger_Vector()
{
    auto& Mgr = ReflManager::GetByRef();
    REGISTER_MATH_TYPE(Vector2i, Construct_Vector2i);
    REGISTER_MATH_TYPE(Vector2f, Construct_Vector2f);
    REGISTER_MATH_TYPE(Vector3i, Construct_Vector3i);
    REGISTER_MATH_TYPE(Vector3f, Construct_Vector3i);
    REGISTER_MATH_TYPE(Vector4i, Construct_Vector4i);
    REGISTER_MATH_TYPE(Vector4f, Construct_Vector4f);
}
