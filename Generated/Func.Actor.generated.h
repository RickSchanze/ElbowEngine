// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::Actor>(), &func::Actor::REFLECTION_Register_Actor_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::Actor>(), &func::Actor::ConstructAt, &func::Actor::DestroyAt); \

#undef GENERATED_BODY_IMPL_Actor
#define GENERATED_BODY_IMPL_Actor \
static void ConstructAt(void* ptr) { new (ptr) func::Actor(); } \
static void DestroyAt(void* ptr) { static_cast<func::Actor*>(ptr)->~Actor(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::Actor::REFLECTION_Register_Actor_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::Actor>("func.Actor")->Internal_AddParent(TypeOf<core::Object>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
type->Internal_RegisterField("transform_", &func::Actor::transform_, offsetof(func::Actor, transform_)); \
type->Internal_RegisterField("components_", &func::Actor::components_, offsetof(func::Actor, components_)); \
return type; \
} \

