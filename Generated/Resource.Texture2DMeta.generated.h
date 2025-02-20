// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Texture2DMeta>(), &resource::Texture2DMeta::REFLECTION_Register_Texture2DMeta_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::Texture2DMeta>(), &resource::Texture2DMeta::ConstructAt, &resource::Texture2DMeta::DestroyAt); \

#undef GENERATED_BODY_IMPL_Texture2DMeta
#define GENERATED_BODY_IMPL_Texture2DMeta \
static void ConstructAt(void* ptr) { new (ptr) resource::Texture2DMeta(); } \
static void DestroyAt(void* ptr) { static_cast<resource::Texture2DMeta*>(ptr)->~Texture2DMeta(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Texture2DMeta::REFLECTION_Register_Texture2DMeta_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Texture2DMeta>("resource.Texture2DMeta")->SetAttribute(core::Type::ValueAttribute::SQLTable, "Texture2D"); \
type->Internal_RegisterField("id", &resource::Texture2DMeta::id, offsetof(resource::Texture2DMeta, id))->SetAttribute(core::FieldInfo::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)"); \
type->Internal_RegisterField("object_handle", &resource::Texture2DMeta::object_handle, offsetof(resource::Texture2DMeta, object_handle)); \
type->Internal_RegisterField("path", &resource::Texture2DMeta::path, offsetof(resource::Texture2DMeta, path)); \
type->Internal_RegisterField("format", &resource::Texture2DMeta::format, offsetof(resource::Texture2DMeta, format)); \
type->Internal_RegisterField("sprites_string", &resource::Texture2DMeta::sprites_string, offsetof(resource::Texture2DMeta, sprites_string))->SetComment("这个Texture2D包含的所有Sprite"); \
return type; \
} \

