/**
 * @file Interface.h
 * @author Echo 
 * @Date 24-11-5
 * @brief 
 */

#pragma once
#include "Reflection/ITypeGetter.h"

namespace core
{
class Interface : public ITypeGetter
{
public:
    typedef ThisClass Super;
    typedef Interface ThisClass;
};
}   // namespace core
