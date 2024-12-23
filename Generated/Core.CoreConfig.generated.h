// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::MemoryConfig>(), &core::MemoryConfig::REFLECTION_Register_MemoryConfig_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::MemoryConfig>(), &core::MemoryConfig::ConstructAt, &core::MemoryConfig::DestroyAt); \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::Version>(), &core::Version::REFLECTION_Register_Version_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::Version>(), &core::Version::ConstructAt, &core::Version::DestroyAt); \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::CoreConfig>(), &core::CoreConfig::REFLECTION_Register_CoreConfig_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::CoreConfig>(), &core::CoreConfig::ConstructAt, &core::CoreConfig::DestroyAt); \

#undef GENERATED_BODY_IMPL_MemoryConfig
#define GENERATED_BODY_IMPL_MemoryConfig \
static void ConstructAt(void* ptr) { new (ptr) core::MemoryConfig(); } \
static void DestroyAt(void* ptr) { static_cast<core::MemoryConfig*>(ptr)->~MemoryConfig(); } \

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
core::Type* core::MemoryConfig::REFLECTION_Register_MemoryConfig_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::MemoryConfig>("core.MemoryConfig")->Internal_AddParent(TypeOf<core::IConfig>())->SetComment("TODO: 或许可以文件分的不那么细?")->SetAttribute(core::Type::ValueAttribute::Config, "Config/Core/MemoryConfig.cfg")->SetAttribute(core::Type::ValueAttribute::Category, "Core"); \
type->Internal_RegisterField("frame_allocator_size", &core::MemoryConfig::frame_allocator_size, offsetof(core::MemoryConfig, frame_allocator_size))->SetComment("帧分配器的内存大小, 默认大小为1MiB"); \
type->Internal_RegisterField("double_frame_allocator_size", &core::MemoryConfig::double_frame_allocator_size, offsetof(core::MemoryConfig, double_frame_allocator_size)); \
return type; \
} \
core::Type* core::Version::REFLECTION_Register_Version_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::Version>("core.Version")->SetAttribute(Type::Trivial); \
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
type->Internal_RegisterField("thread_slot_count", &core::CoreConfig::thread_slot_count, offsetof(core::CoreConfig, thread_slot_count))->SetComment("各ThreadSlot对应的线程数")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "Thread"); \
type->Internal_RegisterField("enable_persistent_load_multithread", &core::CoreConfig::enable_persistent_load_multithread, offsetof(core::CoreConfig, enable_persistent_load_multithread))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "启用多线程持久化对象加载"); \
return type; \
} \

