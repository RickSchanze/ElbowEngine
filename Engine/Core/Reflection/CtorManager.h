/**
 * @file CtorManager.h
 * @author Echo 
 * @Date 24-11-8
 * @brief 
 */

#pragma once
#include "Base/CoreTypeDef.h"
#include "Reflection.h"

namespace core
{
typedef void (*InplaceCtor)(void*);

class CtorManager : public Singleton<CtorManager>
{
public:
    void RegisterCtor(RTTITypeInfo info, InplaceCtor ctor);
    void ConstructAt(const Type* info, void* ptr) const;

private:
    HashMap<RTTITypeInfo, InplaceCtor> ctors_;
};
}   // namespace core