// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Mesh>(), &resource::Mesh::REFLECTION_Register_Mesh_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::Mesh>(), &resource::Mesh::ConstructAt, &resource::Mesh::DestroyAt); \

#undef GENERATED_BODY_IMPL_Mesh
#define GENERATED_BODY_IMPL_Mesh \
static void ConstructAt(void* ptr) { new (ptr) resource::Mesh(); } \
static void DestroyAt(void* ptr) { static_cast<resource::Mesh*>(ptr)->~Mesh(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Mesh::REFLECTION_Register_Mesh_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Mesh>("resource.Mesh")->Internal_AddParent(TypeOf<resource::Asset>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
return type; \
} \

