﻿// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::Version>(), &core::Version::REFLECTION_Register_Version_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::Version>(), &core::Version::ConstructAt, &core::Version::DestroyAt); \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::CoreConfig>(), &core::CoreConfig::REFLECTION_Register_CoreConfig_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::CoreConfig>(), &core::CoreConfig::ConstructAt, &core::CoreConfig::DestroyAt); \

#undef GENERATED_BODY_IMPL_Version
#define GENERATED_BODY_IMPL_Version \
static void ConstructAt(void* ptr) { new (ptr) core::Version(); } \
static void DestroyAt(void* ptr) { static_cast<core::Version*>(ptr)->~Version(); } \

#undef GENERATED_BODY_IMPL_CoreConfig
#define GENERATED_BODY_IMPL_CoreConfig \
static void ConstructAt(void* ptr) { new (ptr) core::CoreConfig(); } \
static void DestroyAt(void* ptr) { static_cast<core::CoreConfig*>(ptr)->~CoreConfig(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* core::Version::REFLECTION_Register_Version_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::Version>("core.Version"); \
type->Internal_RegisterField("major", &core::Version::major, offsetof(core::Version, major)); \
type->Internal_RegisterField("minor", &core::Version::minor, offsetof(core::Version, minor)); \
type->Internal_RegisterField("patch", &core::Version::patch, offsetof(core::Version, patch)); \
return type; \
} \
core::Type* core::CoreConfig::REFLECTION_Register_CoreConfig_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::CoreConfig>("core.CoreConfig")->Internal_AddParent(TypeOf<core::IConfig>())->SetAttribute(core::Type::ValueAttribute::Config, "Config/Core/Core.cfg")->SetAttribute(core::Type::ValueAttribute::Category, "Core"); \
type->Internal_RegisterField("app_name", &core::CoreConfig::app_name, offsetof(core::CoreConfig, app_name))->SetComment("应用的名称"); \
type->Internal_RegisterField("app_version", &core::CoreConfig::app_version, offsetof(core::CoreConfig, app_version))->SetComment("应用版本号"); \
type->Internal_RegisterField("anonymous_thread_count", &core::CoreConfig::anonymous_thread_count, offsetof(core::CoreConfig, anonymous_thread_count))->SetComment("各ThreadSlot对应的线程数")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Thread"); \
type->Internal_RegisterField("tick_frame_interval", &core::CoreConfig::tick_frame_interval, offsetof(core::CoreConfig, tick_frame_interval))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "需要Tick的Manager进行Tick的帧间隔"); \
return type; \
} \

