/**
 * @file ITypeGetter.h
 * @author Echo 
 * @Date 24-10-28
 * @brief 
 */

#pragma once

namespace core
{
struct Type;
class ITypeGetter
{
public:
    virtual ~ITypeGetter() = default;

    [[nodiscard]] virtual const Type* GetType() const = 0;
};
}   // namespace core
