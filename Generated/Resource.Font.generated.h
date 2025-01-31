// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<resource::Font>(), &resource::Font::REFLECTION_Register_Font_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<resource::Font>(), &resource::Font::ConstructAt, &resource::Font::DestroyAt); \

#undef GENERATED_BODY_IMPL_Font
#define GENERATED_BODY_IMPL_Font \
static void ConstructAt(void* ptr) { new (ptr) resource::Font(); } \
static void DestroyAt(void* ptr) { static_cast<resource::Font*>(ptr)->~Font(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* resource::Font::REFLECTION_Register_Font_Registerer() { \
using namespace core; \
Type* type = Type::Create<resource::Font>("resource.Font")->Internal_AddParent(TypeOf<resource::Asset>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
return type; \
} \

