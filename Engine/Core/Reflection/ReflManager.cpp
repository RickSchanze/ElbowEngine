//
// Created by Echo on 2025/3/25.
//
#include "ReflManager.hpp"
#include <cstdint>
#include <ranges>

#include "Core/Profile.hpp"

static void Ctor_String(void *ptr) { new (ptr) String(); }
static void Dtor_String(void *ptr) { static_cast<String *>(ptr)->~String(); }
static void Ctor_Int8(void *ptr) { new (ptr) int8_t(0); }
static void Dtor_Int8(void *ptr) {}
static void Ctor_Int16(void *ptr) { new (ptr) int16_t(0); }
static void Dtor_Int16(void *ptr) {}
static void Ctor_Int32(void *ptr) { new (ptr) int32_t(0); }
static void Dtor_Int32(void *ptr) {}
static void Ctor_Int64(void *ptr) { new (ptr) int64_t(0); }
static void Dtor_Int64(void *ptr) {}
static void Ctor_UInt8(void *ptr) { new (ptr) uint8_t(0); }
static void Dtor_UInt8(void *ptr) {}
static void Ctor_UInt16(void *ptr) { new (ptr) uint16_t(0); }
static void Dtor_UInt16(void *ptr) {}
static void Ctor_UInt32(void *ptr) { new (ptr) uint32_t(0); }
static void Dtor_UInt32(void *ptr) {}
static void Ctor_UInt64(void *ptr) { new (ptr) uint64_t(0); }
static void Dtor_UInt64(void *ptr) {}
static void Ctor_Float(void *ptr) { new (ptr) float(0); }
static void Dtor_Float(void *ptr) {}
static void Ctor_Double(void *ptr) { new (ptr) double(0); }
static void Dtor_Double(void *ptr) {}
static void Ctor_Bool(void *ptr) { new (ptr) bool(false); }
static void Dtor_Bool(void *ptr) {}
static void Ctor_StringView(void *ptr) { new (ptr) StringView(); }
static void Dtor_StringView(void *ptr) { static_cast<StringView *>(ptr)->~StringView(); }

void ReflManager::Startup() {
    ProfileScope _(__func__);
    RegisterType(RTTITypeInfo::Create<Int8>(), Type::Create<Int8>("Int8"));
    RegisterType(RTTITypeInfo::Create<Int16>(), Type::Create<Int16>("Int16"));
    RegisterType(RTTITypeInfo::Create<Int32>(), Type::Create<Int32>("Int32"));
    RegisterType(RTTITypeInfo::Create<Int64>(), Type::Create<Int64>("Int64"));
    RegisterType(RTTITypeInfo::Create<UInt8>(), Type::Create<UInt8>("UInt8"));
    RegisterType(RTTITypeInfo::Create<UInt16>(), Type::Create<UInt16>("UInt16"));
    RegisterType(RTTITypeInfo::Create<UInt32>(), Type::Create<UInt32>("UInt32"));
    RegisterType(RTTITypeInfo::Create<UInt64>(), Type::Create<UInt64>("UInt64"));
    RegisterType(RTTITypeInfo::Create<Float>(), Type::Create<Float>("Float"));
    RegisterType(RTTITypeInfo::Create<Double>(), Type::Create<Double>("Double"));
    RegisterType(RTTITypeInfo::Create<Bool>(), Type::Create<Double>("Bool"));
    RegisterType(RTTITypeInfo::Create<String>(), Type::Create<String>("String"));
    RegisterType(RTTITypeInfo::Create<StringView>(), Type::Create<StringView>("Vector2"));

    RegisterCtorDtor(RTTITypeInfo::Create<String>(), Ctor_String, Dtor_String);
    RegisterCtorDtor(RTTITypeInfo::Create<Int8>(), Ctor_Int8, Dtor_Int8);
    RegisterCtorDtor(RTTITypeInfo::Create<Int16>(), Ctor_Int16, Dtor_Int16);
    RegisterCtorDtor(RTTITypeInfo::Create<Int32>(), Ctor_Int32, Dtor_Int32);
    RegisterCtorDtor(RTTITypeInfo::Create<Int64>(), Ctor_Int64, Dtor_Int64);
    RegisterCtorDtor(RTTITypeInfo::Create<UInt8>(), Ctor_UInt8, Dtor_UInt8);
    RegisterCtorDtor(RTTITypeInfo::Create<UInt32>(), Ctor_UInt32, Dtor_UInt32);
    RegisterCtorDtor(RTTITypeInfo::Create<UInt64>(), Ctor_UInt64, Dtor_UInt64);
    RegisterCtorDtor(RTTITypeInfo::Create<Float>(), Ctor_Float, Dtor_Float);
    RegisterCtorDtor(RTTITypeInfo::Create<Double>(), Ctor_Double, Dtor_Double);
    RegisterCtorDtor(RTTITypeInfo::Create<Bool>(), Ctor_Bool, Dtor_Bool);
    RegisterCtorDtor(RTTITypeInfo::Create<StringView>(), Ctor_StringView, Dtor_StringView);
}

void ReflManager::Shutdown() {
    for (auto &val: mTypesRegistered | std::views::values) {
        Delete(val);
    }
    mTypesRegistered.Clear();
}

void ReflManager::RegisterType(const RTTITypeInfo &type_info) {
    ProfileScope _(__func__);
    if (mTypesRegistered.Contains(type_info)) {
        return;
    }
    if (!mMetaDataRegisters.Contains(type_info)) {
        Log(Fatal) << String::Format("类型{}没有对应的注册函数", *type_info.name);
        return;
    }
    mTypesRegistered[type_info] = mMetaDataRegisters[type_info]();
    mMetaDataRegisters.Remove(type_info);
}

void ReflManager::RegisterTypeRegisterer(const RTTITypeInfo &type_info, MetaDataRegisterer registerer) {
    ProfileScope _(__func__);
    if (registerer == nullptr)
        return;
    if (mMetaDataRegisters.Contains(type_info)) {
        if (mMetaDataRegisters[type_info] != registerer) {
            VLOG_FATAL("类型", *type_info.name, "有不同的注册函数注册");
        }
        return;
    }
    mMetaDataRegisters[type_info] = registerer;
}

void ReflManager::RegisterType(const RTTITypeInfo &type_info, Type *type) {
    ProfileScope _(__func__);
    if (mTypesRegistered.Contains(type_info)) {
        Log(Error) << String::Format("类型{}已经注册过了", *type_info.name);
        return;
    }
    mTypesRegistered[type_info] = type;
}

Type *ReflManager::GetType(const RTTITypeInfo &type_info) {
    if (mTypesRegistered.Contains(type_info)) {
        return mTypesRegistered[type_info];
    }
    RegisterType(type_info);
    if (mTypesRegistered.Contains(type_info)) {
        return mTypesRegistered[type_info];
    }
    VLOG_FATAL("类型", *type_info.name, "没有注册成功");
    return nullptr;
}

void ReflManager::RegisterCtorDtor(const RTTITypeInfo &info, InplaceCtor ctor, InplaceDtor dtor) {
    if (mCtors.Contains(info)) {
        if (mCtors[info].ctor != ctor || mCtors[info].dtor != dtor) {
            VLOG_FATAL("类型", *info.name, "有多个构造函数和析构函数的注册");
        }
        return;
    }
    mCtors[info] = {ctor, dtor};
}

bool ReflManager::ConstructAt(const Type *info, void *ptr) const {
    if (info == nullptr) {
        Log(Error) << "info is nullptr";
        return false;
    }
    if (ptr == nullptr) {
        Log(Error) << "ptr is nullptr";
        return false;
    }
    if (info->IsEnumType()) {
        *static_cast<uint8_t *>(ptr) = 0;
        return true;
    }
    for (auto &[key, value]: mCtors) {
        if (key.hash_code == info->GetHashCode()) {
            value.ctor(ptr);
            return true;
        }
    }
    Log(Error) << String::Format("类型{}没有对应的构造函数", *info->GetName());
    return false;
}

bool ReflManager::DestroyAt(const Type *info, void *ptr) const {
    if (info == nullptr) {
        Log(Error) << "info is nullptr";
        return false;
    }
    if (ptr == nullptr) {
        Log(Error) << "ptr is nullptr";
        return false;
    }
    if (info->IsEnumType()) {
        *static_cast<uint8_t *>(ptr) = 0;
        return true;
    }
    for (auto &[key, value]: mCtors) {
        if (key.hash_code == info->GetHashCode()) {
            value.dtor(ptr);
            return true;
        }
    }
    Log(Error) << String::Format("类型{}没有对应的析构函数", *info->GetName());
    return false;
}
