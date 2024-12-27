// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::ShaderMeta>(), &resource::ShaderMeta::REFLECTION_Register_ShaderMeta_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::ShaderMeta>(), &resource::ShaderMeta::ConstructAt, &resource::ShaderMeta::DestroyAt); \

#undef GENERATED_BODY_IMPL_ShaderMeta
#define GENERATED_BODY_IMPL_ShaderMeta \
static void ConstructAt(void* ptr) { new (ptr) resource::ShaderMeta(); } \
static void DestroyAt(void* ptr) { static_cast<resource::ShaderMeta*>(ptr)->~ShaderMeta(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::ShaderMeta::REFLECTION_Register_ShaderMeta_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::ShaderMeta>("resource.ShaderMeta")->SetAttribute(core::Type::ValueAttribute::SQLTable, "Shader"); \
type->Internal_RegisterField("id", &resource::ShaderMeta::id, offsetof(resource::ShaderMeta, id))->SetAttribute(core::FieldInfo::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)"); \
type->Internal_RegisterField("object_handle", &resource::ShaderMeta::object_handle, offsetof(resource::ShaderMeta, object_handle)); \
type->Internal_RegisterField("path", &resource::ShaderMeta::path, offsetof(resource::ShaderMeta, path)); \
return type; \
} \

