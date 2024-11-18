/**
 * @file CtorManager.cpp
 * @author Echo 
 * @Date 24-11-8
 * @brief 
 */

#include "CtorManager.h"

// clang-format off
static void Ctor_String(void* ptr) { new (ptr) core::String();}
static void Dtor_String(void* ptr) { static_cast<core::String*>(ptr)->~String();}
static void Ctor_Int8(void* ptr) { new (ptr) int8_t(0);}
static void Dtor_Int8(void* ptr) { }
static void Ctor_Int16(void* ptr) { new (ptr) int16_t(0);}
static void Dtor_Int16(void* ptr) { }
static void Ctor_Int32(void* ptr) { new (ptr) int32_t(0);}
static void Dtor_Int32(void* ptr) { }
static void Ctor_Int64(void* ptr) { new (ptr) int64_t(0);}
static void Dtor_Int64(void* ptr) { }
static void Ctor_UInt8(void* ptr) { new (ptr) uint8_t(0);}
static void Dtor_UInt8(void* ptr) { }
static void Ctor_UInt16(void* ptr) { new (ptr) uint16_t(0);}
static void Dtor_UInt16(void* ptr) { }
static void Ctor_UInt32(void* ptr) { new (ptr) uint32_t(0);}
static void Dtor_UInt32(void* ptr) { }
static void Ctor_UInt64(void* ptr) { new (ptr) uint64_t(0);}
static void Dtor_UInt64(void* ptr) { }
static void Ctor_Float(void* ptr) { new (ptr) float(0);}
static void Dtor_Float(void* ptr) { }
static void Ctor_Double(void* ptr) { new (ptr) double(0);}
static void Dtor_Double(void* ptr) { }
static void Ctor_Bool(void* ptr) { new (ptr) bool(false); }
static void Dtor_Bool(void* ptr) { }
static void Ctor_StringView(void* ptr) { new (ptr) core::StringView();}
static void Dtor_StringView(void* ptr) { static_cast<core::StringView*>(ptr)->~StringView();}
// clang-format on

core::CtorManager::CtorManager()
{
    RegisterCtorDtor(RTTITypeInfo::Create<core::String>(), Ctor_String, Dtor_String);
    RegisterCtorDtor(RTTITypeInfo::Create<int8_t>(), Ctor_Int8, Dtor_Int8);
    RegisterCtorDtor(RTTITypeInfo::Create<int16_t>(), Ctor_Int16, Dtor_Int16);
    RegisterCtorDtor(RTTITypeInfo::Create<int32_t>(), Ctor_Int32, Dtor_Int32);
    RegisterCtorDtor(RTTITypeInfo::Create<int64_t>(), Ctor_Int64, Dtor_Int64);
    RegisterCtorDtor(RTTITypeInfo::Create<uint8_t>(), Ctor_UInt8, Dtor_UInt8);
    RegisterCtorDtor(RTTITypeInfo::Create<uint16_t>(), Ctor_UInt16, Dtor_UInt16);
    RegisterCtorDtor(RTTITypeInfo::Create<uint32_t>(), Ctor_UInt32, Dtor_UInt32);
    RegisterCtorDtor(RTTITypeInfo::Create<uint64_t>(), Ctor_UInt64, Dtor_UInt64);
    RegisterCtorDtor(RTTITypeInfo::Create<float>(), Ctor_Float, Dtor_Float);
    RegisterCtorDtor(RTTITypeInfo::Create<double>(), Ctor_Double, Dtor_Double);
    RegisterCtorDtor(RTTITypeInfo::Create<bool>(), Ctor_Bool, Dtor_Bool);
    RegisterCtorDtor(RTTITypeInfo::Create<core::StringView>(), Ctor_StringView, Dtor_StringView);
}

void core::CtorManager::RegisterCtorDtor(RTTITypeInfo info, InplaceCtor ctor, InplaceDtor dtor)
{
    if (ctors_.contains(info))
    {
        LOGGER.Warn(logcat::Reflection, "Duplicated ctor registration for type: {}", info.name);
    }
    ctors_[info] = {ctor, dtor};
}

bool core::CtorManager::ConstructAt(const Type* info, void* ptr) const
{
    if (info == nullptr)
    {
        LOGGER.Error(logcat::Reflection, "type is nullptr");
        return false;
    }
    if (ptr == nullptr)
    {
        LOGGER.Error(logcat::Reflection, "ptr is nullptr");
        return false;
    }
    for (auto& [key, value]: ctors_)
    {
        if (key.hash_code == info->GetTypeHash())
        {
            value.ctor(ptr);
            return true;
        }
    }
    LOGGER.Error(logcat::Reflection, "No ctor registered for type: {}", info->GetFullName());
    return false;
}

bool core::CtorManager::DestroyAt(const Type* info, void* ptr) const
{
    if (info == nullptr)
    {
        LOGGER.Error(logcat::Reflection, "type is nullptr");
        return false;
    }
    if (ptr == nullptr)
    {
        LOGGER.Error(logcat::Reflection, "ptr is nullptr");
        return false;
    }
    for (auto& [key, value]: ctors_)
    {
        if (key.hash_code == info->GetTypeHash())
        {
            value.dtor(ptr);
            return true;
        }
    }
    LOGGER.Error(logcat::Reflection, "No ctor registered for type: {}", info->GetFullName());
    return false;
}
