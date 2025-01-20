// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Texture2D>(), &resource::Texture2D::REFLECTION_Register_Texture2D_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::Texture2D>(), &resource::Texture2D::ConstructAt, &resource::Texture2D::DestroyAt); \

#undef GENERATED_BODY_IMPL_Texture2D
#define GENERATED_BODY_IMPL_Texture2D \
static void ConstructAt(void* ptr) { new (ptr) resource::Texture2D(); } \
static void DestroyAt(void* ptr) { static_cast<resource::Texture2D*>(ptr)->~Texture2D(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Texture2D::REFLECTION_Register_Texture2D_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Texture2D>("resource.Texture2D")->Internal_AddParent(TypeOf<resource::Asset>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
return type; \
} \

