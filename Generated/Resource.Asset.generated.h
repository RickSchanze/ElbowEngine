// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Asset>(), &resource::Asset::REFLECTION_Register_Asset_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::Asset>(), &resource::Asset::ConstructAt, &resource::Asset::DestroyAt); \

#undef GENERATED_BODY_IMPL_Asset
#define GENERATED_BODY_IMPL_Asset \
static void ConstructAt(void* ptr) { new (ptr) resource::Asset(); } \
static void DestroyAt(void* ptr) { static_cast<resource::Asset*>(ptr)->~Asset(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Asset::REFLECTION_Register_Asset_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Asset>("resource.Asset")->Internal_AddParent(TypeOf<core::PersistentObject>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
return type; \
} \

