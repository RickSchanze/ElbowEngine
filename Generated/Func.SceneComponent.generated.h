// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<func::SceneComponent>(), &func::SceneComponent::REFLECTION_Register_SceneComponent_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<func::SceneComponent>(), &func::SceneComponent::ConstructAt, &func::SceneComponent::DestroyAt); \

#undef GENERATED_BODY_IMPL_SceneComponent
#define GENERATED_BODY_IMPL_SceneComponent \
static void ConstructAt(void* ptr) { new (ptr) func::SceneComponent(); } \
static void DestroyAt(void* ptr) { static_cast<func::SceneComponent*>(ptr)->~SceneComponent(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* func::SceneComponent::REFLECTION_Register_SceneComponent_Registerer() { \
using namespace core; \
Type* type = Type::Create<func::SceneComponent>("func.SceneComponent")->Internal_AddParent(TypeOf<func::Component>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
type->Internal_RegisterField("transform_", &func::SceneComponent::transform_, offsetof(func::SceneComponent, transform_)); \
return type; \
} \

