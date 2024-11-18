/**
 * @file CtorManager.h
 * @author Echo 
 * @Date 24-11-8
 * @brief 
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Reflection.h"

namespace core
{
typedef void (*InplaceCtor)(void*);
typedef void (*InplaceDtor)(void*);

struct CtorDtor
{
    InplaceCtor ctor;
    InplaceDtor dtor;
};

class CtorManager : public Singleton<CtorManager>
{
public:
    CtorManager();

    void RegisterCtorDtor(RTTITypeInfo info, InplaceCtor ctor, InplaceDtor dtor);
    bool ConstructAt(const Type* info, void* ptr) const;
    bool DestroyAt(const Type* info, void* ptr) const;

private:
    HashMap<RTTITypeInfo, CtorDtor> ctors_;

};
}   // namespace core
