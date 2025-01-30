// 此文件由工具自动生成，请勿手动修改
#undef GENERATED_ALL_CLASS_BODY
#define GENERATED_ALL_CLASS_BODY \
using namespace core; \
core::MetaInfoManager::Get()->RegisterTypeRegisterer(core::RTTITypeInfo::Create<Font>(), &Font::REFLECTION_Register_Font_Registerer); \
core::CtorManager::Get()->RegisterCtorDtor(RTTITypeInfo::Create<Font>(), &Font::ConstructAt, &Font::DestroyAt); \

#undef GENERATED_BODY_IMPL_Font
#define GENERATED_BODY_IMPL_Font \
static void ConstructAt(void* ptr) { new (ptr) Font(); } \
static void DestroyAt(void* ptr) { static_cast<Font*>(ptr)->~Font(); } \


#undef GENERATED_IMPLEMENTATION
#define GENERATED_IMPLEMENTATION \
core::Type* Font::REFLECTION_Register_Font_Registerer() { \
using namespace core; \
Type* type = Type::Create<Font>("Font")->Internal_AddParent(TypeOf<resource::Asset>())->SetComment("Object不自动生成默认构造函数\nTODO: Destroy"); \
return type; \
} \

