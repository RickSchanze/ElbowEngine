//
// Created by kita on 25-4-29.
//

#include "Matrix.hpp"

#define REGISTER_MATH_TYPE(name, type_register)                                                                                                      \
    {                                                                                                                                                \
        auto type_info = RTTITypeInfo::Create<name>();                                                                                               \
        Mgr.RegisterCtorDtor(type_info, &name::ConstructSelf, &name::DestructSelf);                                                                  \
        Mgr.RegisterTypeRegisterer(type_info, type_register);                                                                                        \
    }

MathTypeRegTrigger_Matrix::MathTypeRegTrigger_Matrix()
{
    auto& Mgr = ReflManager::GetByRef();
    REGISTER_MATH_TYPE(Matrix4x4f, Construct_Matrix4x4f);
    REGISTER_MATH_TYPE(Matrix4x4d, Construct_Matrix4x4d);
    REGISTER_MATH_TYPE(Matrix3x3d, Construct_Matrix3x3d);
    REGISTER_MATH_TYPE(Matrix3x3d, Construct_Matrix3x3d);
}