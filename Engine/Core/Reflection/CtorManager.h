/**
 * @file CtorManager.h
 * @author Echo 
 * @Date 24-11-8
 * @brief 
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Singleton/MManager.h"
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

class CtorManager : public Manager<CtorManager>
{
public:
    CtorManager();

    void                       RegisterCtorDtor(RTTITypeInfo info, InplaceCtor ctor, InplaceDtor dtor);
    bool                       ConstructAt(const Type* info, void* ptr) const;
    bool                       DestroyAt(const Type* info, void* ptr) const;
    [[nodiscard]] ManagerLevel GetLevel() const override { return ManagerLevel::L3; }
    [[nodiscard]] StringView   GetName() const override { return "CtorManager"; }

private:
    HashMap<RTTITypeInfo, CtorDtor> ctors_;
};
}   // namespace core
