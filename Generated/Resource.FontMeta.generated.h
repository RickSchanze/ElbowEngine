// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::FontMeta>(), &resource::FontMeta::REFLECTION_Register_FontMeta_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::FontMeta>(), &resource::FontMeta::ConstructAt, &resource::FontMeta::DestroyAt); \

#undef GENERATED_BODY_IMPL_FontMeta
#define GENERATED_BODY_IMPL_FontMeta \
static void ConstructAt(void* ptr) { new (ptr) resource::FontMeta(); } \
static void DestroyAt(void* ptr) { static_cast<resource::FontMeta*>(ptr)->~FontMeta(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::FontMeta::REFLECTION_Register_FontMeta_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::FontMeta>("resource.FontMeta")->SetAttribute(core::Type::ValueAttribute::SQLTable, "Font"); \
type->Internal_RegisterField("id", &resource::FontMeta::id, offsetof(resource::FontMeta, id))->SetComment("GENERATED_CLASS(FontMeta)")->SetAttribute(core::FieldInfo::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)"); \
type->Internal_RegisterField("object_handle", &resource::FontMeta::object_handle, offsetof(resource::FontMeta, object_handle)); \
type->Internal_RegisterField("path", &resource::FontMeta::path, offsetof(resource::FontMeta, path)); \
return type; \
} \

