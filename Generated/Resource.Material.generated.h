// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Material>(), &resource::Material::REFLECTION_Register_Material_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::Material>(), &resource::Material::ConstructAt, &resource::Material::DestroyAt); \

#undef GENERATED_BODY_IMPL_Material
#define GENERATED_BODY_IMPL_Material \
static void ConstructAt(void* ptr) { new (ptr) resource::Material(); } \
static void DestroyAt(void* ptr) { static_cast<resource::Material*>(ptr)->~Material(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Material::REFLECTION_Register_Material_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Material>("resource.Material")->Internal_AddParent(TypeOf<resource::Asset>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
type->Internal_RegisterField("shader_", &resource::Material::shader_, offsetof(resource::Material, shader_)); \
type->Internal_RegisterField("float3_params_", &resource::Material::float3_params_, offsetof(resource::Material, float3_params_)); \
type->Internal_RegisterField("texture_params_", &resource::Material::texture_params_, offsetof(resource::Material, texture_params_)); \
return type; \
} \

