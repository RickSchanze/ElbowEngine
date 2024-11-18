/**
 * @file Archive.h
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#pragma once

#include "Core/Base/Ref.h"
#include "Core/CoreGlobal.h"
#include "Core/Reflection/CtorManager.h"
#include "Core/Reflection/MetaInfoManager.h"
#include "Core/Reflection/Reflection.h"

namespace core
{
struct Type;
}
namespace core
{
struct Any;
class ITypeGetter;
}   // namespace core
namespace core
{

class ISerializer;

class Archive
{
public:
    virtual ~Archive() = default;

    virtual bool Serialize(const Any& obj, String& out) = 0;

    /// 将source内容反序列化为Type对象, 放在target位置
    /// target必须不为nullptr
    virtual bool Deserialize(core::StringView source, void* target, const Type* type) = 0;

    template <typename T>
    T* DeserializePtr(core::StringView source)
    {
        const Type* type = core::TypeOf<T>();
        return static_cast<T*>(DeserializePtr(source, type));
    }

    void* DeserializePtr(core::StringView source, const Type* type)
    {
        void* target = malloc(type->GetSize());
        CtorManager::Get()->ConstructAt(type, target);
        if (Deserialize(source, target, type))
        {
            return target;
        }
        else
        {
            CtorManager::Get()->DestroyAt(type, target);
            return nullptr;
        }
    }
};
}   // namespace core
