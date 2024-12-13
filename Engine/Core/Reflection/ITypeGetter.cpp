/**
 * @file ITypeGetter.cpp
 * @author Echo 
 * @Date 24-12-3
 * @brief 
 */

#include "ITypeGetter.h"

#include "Core/Log/Logger.h"
#include "Reflection.h"


bool core::ITypeGetter::Is(const Type* type) const
{
    Assert::Ensure(logcat::Reflection, GetType() != nullptr, "self type is nullptr");
    return GetType()->IsDerivedFrom(type);
}
