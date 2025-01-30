// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::ObjectRegistry>(), &core::ObjectRegistry::REFLECTION_Register_ObjectRegistry_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::ObjectRegistry>(), &core::ObjectRegistry::ConstructAt, &core::ObjectRegistry::DestroyAt); \

#undef GENERATED_BODY_IMPL_ObjectRegistry
#define GENERATED_BODY_IMPL_ObjectRegistry \
static void ConstructAt(void* ptr) { new (ptr) core::ObjectRegistry(); } \
static void DestroyAt(void* ptr) { static_cast<core::ObjectRegistry*>(ptr)->~ObjectRegistry(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* core::ObjectRegistry::REFLECTION_Register_ObjectRegistry_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::ObjectRegistry>("core.ObjectRegistry")->SetComment("TODO: 多线程安全"); \
type->Internal_RegisterField("next_handle_persistent_", &core::ObjectRegistry::next_handle_persistent_, offsetof(core::ObjectRegistry, next_handle_persistent_))->SetComment("下一个可用的持久化对象的handle"); \
type->Internal_RegisterField("free_handles_", &core::ObjectRegistry::free_handles_, offsetof(core::ObjectRegistry, free_handles_))->SetComment("可用的, 由于删除造成的handle(persistent only)"); \
return type; \
} \

