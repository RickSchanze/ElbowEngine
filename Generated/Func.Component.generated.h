// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::Component>(), &func::Component::REFLECTION_Register_Component_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::Component>(), &func::Component::ConstructAt, &func::Component::DestroyAt); \

#undef GENERATED_BODY_IMPL_Component
#define GENERATED_BODY_IMPL_Component \
static void ConstructAt(void* ptr) { new (ptr) func::Component(); } \
static void DestroyAt(void* ptr) { static_cast<func::Component*>(ptr)->~Component(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::Component::REFLECTION_Register_Component_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::Component>("func.Component")->Internal_AddParent(TypeOf<core::Object>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
type->Internal_RegisterField("owner_", &func::Component::owner_, offsetof(func::Component, owner_)); \
return type; \
} \

