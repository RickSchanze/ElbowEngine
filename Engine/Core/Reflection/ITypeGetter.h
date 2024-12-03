/**
 * @file ITypeGetter.h
 * @author Echo 
 * @Date 24-10-28
 * @brief 
 */

#pragma once
#include "Core/CoreDef.h"
#include "Core/Log/CoreLogCategory.h"

namespace core
{
struct Type;

class ITypeGetter
{
public:
    typedef ITypeGetter ThisClass;

    virtual ~ITypeGetter() = default;

    [[nodiscard]] virtual const Type* GetType() const = 0;

    [[nodiscard]] bool Is(const Type* type) const;
};
}   // namespace core
