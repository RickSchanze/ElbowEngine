// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<core::resource::MeshMeta>(), &core::resource::MeshMeta::REFLECTION_Register_MeshMeta_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<core::resource::MeshMeta>(), &core::resource::MeshMeta::ConstructAt, &core::resource::MeshMeta::DestroyAt); \

#undef GENERATED_BODY_IMPL_MeshMeta
#define GENERATED_BODY_IMPL_MeshMeta \
static void ConstructAt(void* ptr) { new (ptr) core::resource::MeshMeta(); } \
static void DestroyAt(void* ptr) { static_cast<core::resource::MeshMeta*>(ptr)->~MeshMeta(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* core::resource::MeshMeta::REFLECTION_Register_MeshMeta_Registerer() { \
using namespace core; \
Type* type = Type::Create<core::resource::MeshMeta>("core.resource.MeshMeta")->SetAttribute(core::Type::ValueAttribute::SQLTable, "-"); \
type->Internal_RegisterField("id", &core::resource::MeshMeta::id, offsetof(core::resource::MeshMeta, id))->SetAttribute(core::FieldInfo::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)"); \
return type; \
} \

