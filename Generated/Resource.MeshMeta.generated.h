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
Type* type = Type::Create<core::resource::MeshMeta>("core.resource.MeshMeta")->SetAttribute(core::Type::ValueAttribute::SQLTable, "Mesh"); \
type->Internal_RegisterField("id", &core::resource::MeshMeta::id, offsetof(core::resource::MeshMeta, id))->SetAttribute(core::FieldInfo::ValueAttribute::SQLAttr, "(PrimaryKey, AutoIncrement)"); \
type->Internal_RegisterField("object_id", &core::resource::MeshMeta::object_id, offsetof(core::resource::MeshMeta, object_id)); \
type->Internal_RegisterField("path", &core::resource::MeshMeta::path, offsetof(core::resource::MeshMeta, path)); \
type->Internal_RegisterField("triangulate", &core::resource::MeshMeta::triangulate, offsetof(core::resource::MeshMeta, triangulate))->SetComment("将所有网格多边形面转换为三角形面")->SetAttribute(core::FieldInfo::ValueAttribute::Label, "三角化")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "基础"); \
type->Internal_RegisterField("generate_normals", &core::resource::MeshMeta::generate_normals, offsetof(core::resource::MeshMeta, generate_normals))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "生成法线")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "基础"); \
type->Internal_RegisterField("generate_smooth_normals", &core::resource::MeshMeta::generate_smooth_normals, offsetof(core::resource::MeshMeta, generate_smooth_normals))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "生产平滑法线")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "基础"); \
type->Internal_RegisterField("merge_duplicate_vertices", &core::resource::MeshMeta::merge_duplicate_vertices, offsetof(core::resource::MeshMeta, merge_duplicate_vertices))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "合并重复顶点")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "基础"); \
type->Internal_RegisterField("remove_unused_materials", &core::resource::MeshMeta::remove_unused_materials, offsetof(core::resource::MeshMeta, remove_unused_materials))->SetAttribute(core::FieldInfo::ValueAttribute::Label, "移除多余材质")->SetAttribute(core::FieldInfo::ValueAttribute::Category, "基础"); \
type->Internal_RegisterField("can_be_removed", &core::resource::MeshMeta::can_be_removed, offsetof(core::resource::MeshMeta, can_be_removed))->SetAttribute(FieldInfo::Hidden); \
return type; \
} \

