// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::MaterialMeta>(), &resource::MaterialMeta::REFLECTION_Register_MaterialMeta_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::MaterialMeta>(), &resource::MaterialMeta::ConstructAt, &resource::MaterialMeta::DestroyAt); \

#undef GENERATED_BODY_IMPL_MaterialMeta
#define GENERATED_BODY_IMPL_MaterialMeta \
static void ConstructAt(void* ptr) { new (ptr) resource::MaterialMeta(); } \
static void DestroyAt(void* ptr) { static_cast<resource::MaterialMeta*>(ptr)->~MaterialMeta(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::MaterialMeta::REFLECTION_Register_MaterialMeta_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::MaterialMeta>("resource.MaterialMeta")->SetAttribute(core::Type::ValueAttribute::SQLTable, "Material"); \
type->Internal_RegisterField("id", &resource::MaterialMeta::id, offsetof(resource::MaterialMeta, id))->SetAttribute(core::FieldInfo::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)"); \
type->Internal_RegisterField("object_handle", &resource::MaterialMeta::object_handle, offsetof(resource::MaterialMeta, object_handle)); \
type->Internal_RegisterField("path", &resource::MaterialMeta::path, offsetof(resource::MaterialMeta, path)); \
return type; \
} \

