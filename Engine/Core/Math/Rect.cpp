//
// Created by kita on 25-4-27.
//

#include "Rect.hpp"

#define REGISTER_MATH_TYPE(name, type_register)                                                                                                      \
    {                                                                                                                                                \
        auto type_info = RTTITypeInfo::Create<name>();                                                                                               \
        Mgr.RegisterCtorDtor(type_info, &name::ConstructSelf, &name::DestructSelf);                                                                  \
        Mgr.RegisterTypeRegisterer(type_info, type_register);                                                                                        \
    }

MathTypeRegTrigger_Rect::MathTypeRegTrigger_Rect()
{
    auto &Mgr = ReflManager::GetByRef();
    REGISTER_MATH_TYPE(Rect2Di, Construct_Rect2Di);
    REGISTER_MATH_TYPE(Rect2Df, Construct_Rect2Df);
}